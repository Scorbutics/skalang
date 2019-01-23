#include "TypeBuilderBinary.h"

#include "NodeValue/AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BINARY>::build(const SymbolTable& symbols, OperateOn node) {
    assert(!node.GetOperator().empty());
    const auto& type1 = node.GetFirstType();
    const auto& type2 = node.GetSecondType();
	return Type{ type1.crossTypes(node.GetOperator(), type2) };
}
