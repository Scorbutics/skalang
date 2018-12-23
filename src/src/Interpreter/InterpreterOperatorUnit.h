#pragma once

#include "NodeValue/Token.h"
#include "NodeValue.h"

namespace ska {
	class SymbolTable;
	class ASTNode;
	class MemoryTable;

	class InterpreterOperatorUnit {
	public:
        virtual NodeValue interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) = 0;
    };
}
