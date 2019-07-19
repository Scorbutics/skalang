#pragma once
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeValue.h"

namespace ska {
	class BytecodeCell {
	public:
        BytecodeCell(Type type, Operator oper, std::vector<BytecodeValue> parameters) : 
            m_parameters(std::move(parameters)),
            m_operator(oper),
            m_type(std::move(type)) {
        }

		BytecodeCell() = default;

		BytecodeCell(BytecodeCell&&) noexcept = default;
		BytecodeCell& operator=(BytecodeCell&&) noexcept = default;

		BytecodeCell(const BytecodeCell&) = delete;
		BytecodeCell& operator=(const BytecodeCell&) = delete;

	private:
        std::vector<BytecodeValue> m_parameters;
        Operator m_operator = Operator::EMPTY;
        Type m_type;
	};
}
