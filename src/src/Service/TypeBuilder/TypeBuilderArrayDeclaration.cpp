#include "TypeBuilderArrayDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	if (!node.HasArrayType()) {
		return Type::MakeBuiltIn(ExpressionType::VOID);
	}
	auto type = Type::MakeBuiltIn<ExpressionType::ARRAY>();
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return type;
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_TYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	if (!node.HasExplicitArrayType()) {
		return node.GetArrayContent().type().value();
	}
	auto type = Type::MakeBuiltIn<ExpressionType::ARRAY>();
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return type;
}
