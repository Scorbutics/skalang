#include "TypeBuilderVariableAffectation.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::AFFECTATION>::build(const ScriptAST& script, OperateOn node) {
    assert(node.GetVariableNameNode().type().has_value());
    return node.GetVariableNameNode().type().value();
}
