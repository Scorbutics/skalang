#pragma once

#include "NodeValue/ExpressionType.h"

namespace ska {
    class SymbolTable;
    class ASTNode;

	Type TypeBuilderBuildFromTokenType(const SymbolTable& symbols, const ASTNode& node);
}
