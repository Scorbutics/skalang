#pragma once
#include <variant>
#include "NodeValue/Token.h"
#include "NodeValue/Type.h"
#include "BytecodeCommand.h"

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

		BytecodeRValue() = default;

	private:
		BytecodeCommand m_command = BytecodeCommand::NOP;
        Token m_value;
        Type m_type;
	};

	using BytecodeLValue = BytecodeRValue*;

    using BytecodeValue = std::variant<BytecodeLValue, BytecodeRValue>;
}
