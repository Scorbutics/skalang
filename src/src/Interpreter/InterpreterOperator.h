#pragma once

#include "NodeValue/Operator.h"
#include "Interpreter.h"
#include "InterpreterOperatorUnit.h"

#define SKALANG_INTERPRETER_OPERATOR_DEFINE(OperatorType)\
    template<>\
    class InterpreterOperator<OperatorType> : \
		public InterpreterOperatorUnit,\
		public Operation<OperatorType> {\
	public:\
		InterpreterOperator(Interpreter& interpreter) : m_interpreter(interpreter) {} \
        NodeCell interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) override;\
	private:\
		Interpreter& m_interpreter;\
    };

namespace ska {
	class SymbolTable;
	class ASTNode;

	template <Operator O>
	class InterpreterOperator : 
		public InterpreterOperatorUnit {
	public:
		NodeCell interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) override { return ""; }
	};
}
