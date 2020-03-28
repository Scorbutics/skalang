#include "BytecodeOperandSerializer.h"
#include "BytecodeCommonSerializer.h"

#include "Generator/Value/BytecodeOperand.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/BytecodeChunk.h"

const std::string ska::bytecode::OperandSerializer::scriptName(const ScriptCache& cache, std::size_t id) {
	if (!cache.exist(id)) { throw std::runtime_error("script with id \"" + std::to_string(id) + "\" does not exist"); }
	return cache[id].name();
}

void ska::bytecode::OperandSerializer::write(const ScriptCache& cache, SerializerSafeZone<sizeof(uint8_t) + sizeof(Chunk) * 2> output, const Operand& value) {
	uint8_t type = static_cast<uint8_t>(OperandType::EMPTY);
	Chunk script { 0 };
	Chunk variable { 0 };

	if (value.empty()) {
		output.write(type);
		CommonSerializer::writeNullChunk<2>(output.acquireMemory<sizeof(Chunk)*2>("empty script + variable"));
		return;
	}

	const auto& content = value.content();
	if (std::holds_alternative<StringShared>(content)) {
		type = static_cast<uint8_t>(OperandType::PURE);
		script = 0;
		output.write(type);
		output.write(script);
		output.write(*std::get<StringShared>(content));
	} else {
		type = static_cast<uint8_t>(value.type());

		std::visit([&](const auto& operand) {
			using TypeT = std::decay_t<decltype(operand)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				output.write(type);
				output.write(scriptName(cache, operand.script));
				
				if (value.type() == OperandType::BIND_SCRIPT) {
					output.write(scriptName(cache, operand.variable));
				} else {
					output.write(static_cast<Chunk>(operand.variable));
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
				
				output.write(type);
				output.write(script);
				output.write(variable);
			}
		}, content);
	}
}

