#pragma once

#include "NodeValue/ExpressionType.h"

namespace ska {
	class SymbolTable;
	class ASTNode;
	class Parser;

    class TypeBuildUnit {
    public:
        virtual Type build(Parser& parser, const SymbolTable&, const ASTNode&) = 0;
    };
}
