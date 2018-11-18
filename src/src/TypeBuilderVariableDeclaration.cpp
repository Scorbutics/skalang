#include "TypeBuilderVariableDeclaration.h"

#include "AST.h"
#include "SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 1);
    return node[0].type().value();
}
