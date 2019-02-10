#include "TypeBuilderBridge.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BRIDGE>::build(const SymbolTable& symbols, OperateOn node) {
	const auto* symbol = symbols[node.GetValue().name()];
	if (symbol != nullptr) {
		return Type::MakeCustom<ExpressionType::OBJECT>(symbol);
	}
	return Type::MakeBuiltIn(ExpressionType::VOID);
}
