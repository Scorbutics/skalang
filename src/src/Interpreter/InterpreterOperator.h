#pragma once

#include "NodeValue/Operator.h"
#include "Interpreter.h"
#include "InterpreterOperatorUnit.h"
#include "ExecutionContext.h"

#define SKALANG_INTERPRETER_OPERATOR_DEFINE(OperatorType)\
    template<>\
    class InterpreterOperator<OperatorType> : \
		public InterpreterOperatorBase {\
	private:\
		using OperateOn = Operation<OperatorType>;\
	public:\
		using InterpreterOperatorBase::InterpreterOperatorBase;\
        NodeCell interpret(ExecutionContext& node) override final {\
			return interpret(OperateOn{node});\
		}\
		NodeCell interpret(OperateOn node);\
    };

namespace ska {
	class SymbolTable;
	class MemoryTable;
	class ASTNode;

	template <Operator O>
	class InterpreterOperator : 
		public InterpreterOperatorUnit {
	public:
		NodeCell interpret(ExecutionContext node) override { return NodeCell {""}; }
	};
}
