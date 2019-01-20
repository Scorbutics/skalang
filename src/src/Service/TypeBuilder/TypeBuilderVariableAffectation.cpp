#include "TypeBuilderVariableAffectation.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(StatementParser& parser, const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() > 0 && node[0].type().has_value());
    return node[0].type().value();
}
