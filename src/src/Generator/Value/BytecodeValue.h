#pragma once
#include <variant>
#include "NodeValue/Token.h"
#include "NodeValue/Type.h"

namespace ska {
	class BytecodeRValue {
	public:
        BytecodeRValue(Type type, Token value) : 
            m_value(std::move(value)),
            m_type(std::move(type)) {
        }

		BytecodeRValue() = default;

	private:
        Token m_value;
        Type m_type;
	};

    class BytecodeLValue {
    public:
        BytecodeLValue(BytecodeRValue& target) :
            m_target(&target) {
        }

    private:
        BytecodeRValue* m_target = nullptr;
    };

    using BytecodeValue = std::variant<BytecodeRValue, BytecodeLValue>;
}
