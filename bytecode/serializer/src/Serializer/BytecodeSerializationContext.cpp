#include "BytecodeSerializationContext.h"

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
