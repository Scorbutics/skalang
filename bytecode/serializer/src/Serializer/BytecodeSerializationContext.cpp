#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializationContext.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::SerializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SerializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SerializationContext)

void ska::bytecode::SerializationContext::writeHeader(std::size_t serializerVersion) {
	(*this) << serializerVersion;
	(*this) << currentScriptName();
	(*this) << currentScriptId();
	(*this) << static_cast<std::size_t>(currentScriptBridged());
}

std::vector<std::string> ska::bytecode::SerializationContext::writeInstructions() {
	auto linkedScripts = std::vector<std::string>{};
	for (const auto& instruction : (*this)) {
		LOG_DEBUG << "Serializing " << instruction;
		if (instruction.command() == Command::SCRIPT) {
			linkedScripts.push_back(scriptName(instruction.left().as<ScriptVariableRef>().variable));
		}
		(*this) << instruction;
	}
	(*this) << Instruction{ Command::NOP, std::vector<Operand>{} };
	return linkedScripts;
}

void ska::bytecode::SerializationContext::writeExternalReferences(std::vector<std::string> linkedScripts) {
	(*this) << linkedScripts.size();
	for (const auto& linkedScript : linkedScripts) {
		LOG_INFO << linkedScript;
		(*this) << linkedScript;
	}
}

void ska::bytecode::SerializationContext::writeExports() {
	auto& exports = m_cache[m_id].exportedSymbols();
	LOG_INFO << "Export serializing : " << exports.size();
	for (const auto& exp : exports) {
		if (!exp.empty()) {
			LOG_INFO << exp;
			(*this) << exp;
		}
	}
	(*this) << Operand{};
}


std::ostream& ska::bytecode::SerializationContext::operator<<(std::size_t value) {
	m_output->write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
	return *m_output;
}

std::ostream& ska::bytecode::SerializationContext::operator<<(std::string value) {
	const Chunk refIndex = m_natives.size();
	m_output->write(reinterpret_cast<const char*>(&refIndex), sizeof(Chunk));
	m_natives.push_back(std::move(value));
	return *m_output;
}

std::ostream& ska::bytecode::SerializationContext::operator<<(const Instruction& value) {
	uint16_t cmd = static_cast<uint16_t>(value.command());
	uint8_t numberOfValidOperands = !value.dest().empty() + !value.left().empty() + !value.right().empty();
	m_output->write(reinterpret_cast<const char*>(&cmd), sizeof(uint16_t));
	m_output->write(reinterpret_cast<const char*>(&numberOfValidOperands), sizeof(uint8_t));

	if (numberOfValidOperands-- > 0) *this << value.dest(); else return *m_output;
	if (numberOfValidOperands-- > 0) *this << value.left(); else return *m_output;
	if (numberOfValidOperands-- > 0) *this << value.right();
	return *m_output;
}

std::ostream& ska::bytecode::SerializationContext::operator<<(const Operand& value) {
	uint8_t type = static_cast<uint8_t>(OperandType::EMPTY);
	Chunk script { 0 };
	Chunk variable { 0 };

	if (value.empty()) {
		m_output->write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		char empty[sizeof(Chunk) * 2] = "";
		m_output->write(empty, sizeof(empty));
		return *m_output;
	}

	const auto& content = value.content();
	if (std::holds_alternative<StringShared>(content)) {
		type = static_cast<uint8_t>(OperandType::PURE);
		script = 0;
		m_output->write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		m_output->write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		*this << *std::get<StringShared>(content);
	} else {
		type = static_cast<uint8_t>(value.type());

		std::visit([&](const auto& operand) {
			using TypeT = std::decay_t<decltype(operand)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				script = operand.script;
				variable = operand.variable;
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

		m_output->write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
		m_output->write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
		m_output->write(reinterpret_cast<const char*>(&variable), sizeof(Chunk));
	}
	return *m_output;
}

void ska::bytecode::SerializationContext::pushNatives() {
	for (auto& native : m_natives) {
		if (! native.empty()) {
			const std::size_t size = native.size();
			m_output->write(reinterpret_cast<const char*>(&size), sizeof(Chunk));
			if (size > 0) {
				m_output->write(native.c_str(), sizeof(char) * size);
			}
		}
	}
	char empty[sizeof(Chunk)] = "";
	m_output->write(empty, sizeof(empty));
	m_natives.clear();
}
