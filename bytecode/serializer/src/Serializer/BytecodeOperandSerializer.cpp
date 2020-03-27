#include "BytecodeOperandSerializer.h"
#include "BytecodeCommonSerializer.h"

#include "Generator/Value/BytecodeOperand.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/BytecodeChunk.h"

const std::string ska::bytecode::OperandSerializer::scriptName(const ScriptCache& cache, std::size_t id) {
	if (!cache.exist(id)) { throw std::runtime_error("script with id \"" + std::to_string(id) + "\" does not exist"); }
	return cache[id].name();
}

void ska::bytecode::OperandSerializer::write(const ScriptCache& cache, std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Operand& value) {
	uint8_t type = static_cast<uint8_t>(OperandType::EMPTY);
	Chunk script { 0 };
	Chunk variable { 0 };

	if (value.empty()) {
		buffer.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		char empty[sizeof(Chunk) * 2] = "";
		buffer.write(empty, sizeof(empty));
		return;
	}

	const auto& content = value.content();
	if (std::holds_alternative<StringShared>(content)) {
		type = static_cast<uint8_t>(OperandType::PURE);
		script = 0;
		buffer.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		buffer.write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		CommonSerializer::write(buffer, *std::get<StringShared>(content));
	} else {
		type = static_cast<uint8_t>(value.type());

		std::visit([&](const auto& operand) {
			using TypeT = std::decay_t<decltype(operand)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				natives.emplace(scriptName(cache, operand.script), natives.size());
				script = natives.at(scriptName(cache, operand.script));
				
				if (value.type() == OperandType::BIND_SCRIPT) {
					natives.emplace(scriptName(cache, operand.variable), natives.size());
					variable = natives.at(scriptName(cache, operand.variable));
				} else {
					variable = operand.variable;
				}
			} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
				if constexpr (std::is_same_v<long, TypeT>) {
					script = static_cast<Chunk>(1);
				} else if constexpr (std::is_same_v<double, TypeT>) {
					script = static_cast<Chunk>(2);
				} else {
					script = static_cast<Chunk>(3);
				}
				variable = static_cast<Chunk>(operand);
			}
		}, content);

		buffer.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		buffer.write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		buffer.write(reinterpret_cast<const char*>(&variable), sizeof(Chunk));
	}
}

