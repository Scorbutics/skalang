#include "TypeBuilderVariableDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const ScriptAST& script, OperateOn node) {
    const auto* symbol = script.symbols()[node.GetVariableName()];
    if (symbol != nullptr && symbol->getType() != ExpressionType::VOID) {
        return symbol->getType();
    }
    
    return node.GetVariableValueNode().type().value();
}
