#pragma once

#include "NodeValue/Token.h"

namespace ska {
	class SymbolTable;
	class ASTNode;
	class MemoryTable;

	class InterpreterOperatorUnit {
	public:
        virtual Token::Variant interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) = 0;
    };
}
