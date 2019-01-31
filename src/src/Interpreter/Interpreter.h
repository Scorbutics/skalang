#pragma once
#include <memory>
#include <vector>
#include "InterpreterOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "MemoryTable.h"

namespace ska {
	class Interpreter {
		using OperatorInterpreter = std::vector<std::unique_ptr<InterpreterOperatorUnit>>;
	public:
		Interpreter(SymbolTable& symbols);
		~Interpreter() = default;

		OperatorInterpreter build();
		NodeCell interpret(ASTNode& node);
		NodeValue script(ASTNode& node);
		
	private:
		SymbolTable& m_symbols;
		MemoryTable m_memory;
		OperatorInterpreter m_operatorInterpreter;
	};
}
