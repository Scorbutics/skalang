#include "TypeBuilderBinary.h"

#include "AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BINARY>::build(const SymbolTable& symbols, ASTNode& node) {
    assert(node.size() == 2 && !node.asString().empty());
    const auto& type1 = TypeBuilderDispatchCalculation(symbols, node[0]);
    const auto& type2 = TypeBuilderDispatchCalculation(symbols, node[1]);
    return type1.crossTypes(node.asString()[0], type2);
}
