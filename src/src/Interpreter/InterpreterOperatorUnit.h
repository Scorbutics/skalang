#pragma once

#include "NodeValue/Token.h"
#include "NodeValue.h"

namespace ska {
	class ASTNode;
	class MemoryTable;
	class SymbolTable;
	class Interpreter;

	class InterpreterOperatorUnit {
	public:
		virtual NodeCell interpret(ASTNode& node) = 0;
	};

	class InterpreterOperatorBase :
		public InterpreterOperatorUnit {
	protected:
		InterpreterOperatorBase(Interpreter& interpreter, const SymbolTable& symbols, MemoryTable& memory) :
			m_interpreter(interpreter),
			m_symbols(symbols),
			m_memory(memory) {}

		Interpreter& m_interpreter;
		const SymbolTable& m_symbols;
		MemoryTable& m_memory;
	};

}
