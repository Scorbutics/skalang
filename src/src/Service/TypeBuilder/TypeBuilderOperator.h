#pragma once

#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "TypeBuildUnit.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Operation/OperationType.h"

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(OperatorType)\
    template<>\
    struct TypeBuilderOperator<OperatorType> : \
		public TypeBuildUnit {\
	private:\
		using OperateOn = OperationType<OperatorType>;\
	public:\
        Type build(const SymbolTable& symbols, const ASTNode& node) override final {\
			return build(symbols, OperateOn{node});\
		}\
		Type build(const SymbolTable& symbols, OperateOn node);\
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
