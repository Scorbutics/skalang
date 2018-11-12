#pragma once

#include "ExpressionType.h"

namespace ska {
    class SymbolTable;
    class ASTNode;

	Type TypeBuilderDispatchCalculation(const SymbolTable& symbols, ASTNode& node);
}
