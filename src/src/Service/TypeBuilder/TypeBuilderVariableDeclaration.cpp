#include "TypeBuilderVariableDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>::build(const SymbolTable& symbols, OperateOn node) {
    return node.GetVariableValueNode().type().value();
}
