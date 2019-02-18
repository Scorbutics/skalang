#include "TypeBuilderBridge.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BRIDGE>::build(const SymbolTable& symbols, OperateOn node) {
	const auto* symbol = symbols[node.GetValue().name()];
	return symbol == nullptr ? Type::MakeBuiltIn(ExpressionType::VOID) : Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}
