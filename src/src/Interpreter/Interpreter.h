#pragma once
#include <memory>
#include <vector>
#include "InterpreterOperatorUnit.h"
#include "NodeValue/ASTNodePtr.h"
#include "MemoryTable.h"

namespace ska {
	class Interpreter {
		using OperatorInterpreter = std::vector<std::unique_ptr<InterpreterOperatorUnit>>;
	public:
		Interpreter(SymbolTable& symbols);
		~Interpreter() = default;
		
		OperatorInterpreter build();
		void interpret(ASTNodePtr root);
		const Token::Variant& interpret(ASTNode& node);

	private:
		SymbolTable& m_symbols;
		OperatorInterpreter m_operatorInterpreter;
		MemoryTable m_memory;
	};
}
