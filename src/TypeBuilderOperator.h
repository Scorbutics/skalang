#pragma once

#include "Operator.h"
#include "ExpressionType.h"

//#define SKALANG_LOG_TYPE_BUILDER_OPERATOR

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(OperatorType)\
    template<>\
    struct TypeBuilderOperator<OperatorType> {\
        static Type build(const SymbolTable& symbols, ASTNode& node);\
    };

namespace ska {
    class ASTNode;
    class SymbolTable;

    template <Operator O>
    struct TypeBuilderOperator {
        static Type build(const SymbolTable& symbols, ASTNode& node) {
            return ExpressionType::VOID;
        }
    };


}
