#pragma once

#include "Operator.h"
#include "Type.h"
#include "TypeBuildUnit.h"
#include "TypeBuilderCalculatorDispatcher.h"

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(OperatorType)\
    template<>\
    struct TypeBuilderOperator<OperatorType> : public TypeBuildUnit {\
        Type build(const SymbolTable& symbols, const ASTNode& node) override;\
    };

namespace ska {
    class ASTNode;
    class SymbolTable;

    template <Operator O>
    struct TypeBuilderOperator : public TypeBuildUnit {
        Type build(const SymbolTable& symbols, const ASTNode& node) override {
			return Type{ ExpressionType::VOID };
        }
    };


}
