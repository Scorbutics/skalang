#pragma once

#include "ExpressionType.h"

namespace ska {
    class SymbolTable;
    class ASTNode;

	const ska::Type& TypeBuilderDispatchCalculation(const SymbolTable& symbols, ASTNode& node);
}
