#include "TypeBuilderVariableAffectation.h"

#include "AST.h"
#include "SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const SymbolTable& symbols, ASTNode& node) {
    const auto varTypeSymbol = symbols[node.asString()];
    assert(varTypeSymbol != nullptr);
    return varTypeSymbol->getType();
}
