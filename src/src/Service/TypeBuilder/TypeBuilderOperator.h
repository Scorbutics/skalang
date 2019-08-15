#pragma once

#include "NodeValue/Type.h"
#include "TypeBuildUnit.h"
#include "Operation/Type/OperationType.h"

#define SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(OperatorType)\
    template<>\
    struct TypeBuilderOperator<OperatorType> : \
		public TypeBuildUnit {\
	private:\
		using OperateOn = OperationType<OperatorType>;\
	public:\
        Type build(const ScriptAST& script, const ASTNode& node) override final {\
			return build(script, OperateOn{node});\
		}\
		Type build(const ScriptAST& script, OperateOn node);\
    };

namespace ska {
    class ASTNode;
    class ScriptAST;

    template <Operator O>
    struct TypeBuilderOperator : public TypeBuildUnit {
        Type build(const ScriptAST& script, const ASTNode& node) override {
			return Type{ };
        }
    };


}

#include "Service/TypeBuilder/TypeBuilderFunctionCall.h"
#include "Service/TypeBuilder/TypeBuilderFunctionPrototypeDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderFunctionDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderParameterDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderArrayDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderArrayUse.h"
#include "Service/TypeBuilder/TypeBuilderVariableAffectation.h"
#include "Service/TypeBuilder/TypeBuilderVariableDeclaration.h"
#include "Service/TypeBuilder/TypeBuilderImport.h"
#include "Service/TypeBuilder/TypeBuilderExport.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/TypeBuilder/TypeBuilderScriptLink.h"
#include "Service/TypeBuilder/TypeBuilderLiteral.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "Service/TypeBuilder/TypeBuilderType.h"
#include "Service/TypeBuilder/TypeBuilderUserDefinedObject.h"
