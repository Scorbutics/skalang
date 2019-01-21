#include "TypeBuilderVariableAffectation.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const SymbolTable& symbols, OperateOn node) {
    assert(node.size() > 0 && node.GetVariableNameNode().type().has_value());
    return node.GetVariableNameNode().type().value();
}
