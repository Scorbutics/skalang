#include "TypeBuilderVariableAffectation.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::AFFECTATION>::build(ScriptAST& script, OperateOn node) {
    return node.GetVariableNameNode().type().value();
}
