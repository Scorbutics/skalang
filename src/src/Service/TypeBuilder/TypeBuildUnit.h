#pragma once

#include "NodeValue/Type.h"

namespace ska {
	class SymbolTable;
	class ASTNode;

    class TypeBuildUnit {
    public:
        virtual Type build(const SymbolTable&, const ASTNode&) = 0;
    };
}
