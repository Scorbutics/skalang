#include "TypeBuilderUnary.h"

#include "AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const SymbolTable& symbols, ASTNode& node) {
    assert(node.size() == 1);
    const auto& childType = TypeBuilderDispatchCalculation(symbols, node[0]);
    return childType;
}
