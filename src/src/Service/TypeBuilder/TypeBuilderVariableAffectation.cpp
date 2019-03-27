#include "TypeBuilderVariableAffectation.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const Script& script, OperateOn node) {
    assert(node.GetVariableNameNode().type().has_value());
    return node.GetVariableNameNode().type().value();
}
