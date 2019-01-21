#pragma once

#include "NodeValue/ExpressionType.h"

namespace ska {
	class SymbolTable;
	class ASTNode;

    class TypeBuildUnit {
    public:
        virtual Type build(const SymbolTable&, const ASTNode&) = 0;
    };
}
