#include "TypeBuilderArrayDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>::build(const SymbolTable& symbols, OperateOn node) {
	auto type = Type{ ExpressionType::ARRAY };
	if (!node.HasArrayType()) {
		return type;
	}
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return type;
}
