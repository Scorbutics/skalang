#include "TypeBuilderUnary.h"

#include "AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 1);
    const auto& childType = node[0].type().value();
    return childType;
}
