#pragma once
#include <variant>
#include "NodeValue/Token.h"
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"

namespace ska {
	class BytecodeRValue {
	public:
        BytecodeRValue(BytecodeCommand command, Type type, Token value) :
			m_command(std::move(command)),
			m_value(std::move(value)),
            m_type(std::move(type)) {
        }

		BytecodeRValue(BytecodeCommand command) :
			m_command(std::move(command)) {
		}

		BytecodeRValue(BytecodeCommand command, const ASTNode& node);

		BytecodeRValue() = default;

		auto command() const { return m_command; }

		BytecodeRValue makeInVariableCell(std::string name) const {
			return { BytecodeCommand::IN, m_type, Token{std::move(name), TokenType::IDENTIFIER, m_value.position() } };
		}

		const auto& value() const {
			return m_value;
		}

		friend std::ostream& operator<<(std::ostream& stream, const BytecodeRValue&);

	private:
		BytecodeCommand m_command = BytecodeCommand::NOP;
		Token m_value;
		Type m_type;
	};

	using BytecodeLValue = BytecodeRValue*;
  using BytecodeValue = std::variant<BytecodeLValue, BytecodeRValue>;

	std::ostream& operator<<(std::ostream& stream, const BytecodeRValue&);
}
