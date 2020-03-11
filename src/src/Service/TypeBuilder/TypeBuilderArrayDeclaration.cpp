#include "TypeBuilderArrayDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>::build(ScriptAST& script, OperateOn node) {
	auto type = Type::MakeBuiltIn<ExpressionType::ARRAY>();
	if (!node.HasArrayType()) {
		return type;
	}
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return type;
}
