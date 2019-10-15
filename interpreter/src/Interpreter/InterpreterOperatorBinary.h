#pragma once

#include "InterpreterOperator.h"
#include "Operation/OperationBinary.h"

namespace ska {
	class TypeCrosser;
	template<>
	class InterpreterOperator<Operator::BINARY> :
		public InterpreterOperatorBase{ \
	private:
		using OperateOn = Operation<Operator::BINARY>;
		const TypeCrosser& m_typeCrosser;
	public:
		InterpreterOperator(Interpreter& interpreter, const TypeCrosser& typeCrosser);
		NodeCell interpret(ExecutionContext& node) override final {
			return interpret(OperateOn{node});
		}
		NodeCell interpret(OperateOn node); 
	};
}
