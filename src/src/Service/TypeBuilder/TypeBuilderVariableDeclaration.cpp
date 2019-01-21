#include "TypeBuilderVariableDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>::build(const SymbolTable& symbols, OperateOn node) {
    assert(node.size() == 1);
    return node[0].type().value();
}
