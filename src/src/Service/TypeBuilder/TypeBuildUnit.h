#pragma once

#include "NodeValue/ExpressionType.h"

namespace ska {
	class SymbolTable;
	class ASTNode;
	class StatementParser;

    class TypeBuildUnit {
    public:
        virtual Type build(StatementParser& parser, const SymbolTable&, const ASTNode&) = 0;
    };
}
