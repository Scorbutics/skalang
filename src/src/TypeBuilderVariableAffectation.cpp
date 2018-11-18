#include "TypeBuilderVariableAffectation.h"

#include "AST.h"
#include "SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() > 0);
    assert(!node[0].asString().empty());
    const auto varTypeSymbol = symbols[node[0].asString()];
    assert(varTypeSymbol != nullptr);
    return varTypeSymbol->getType();
}
