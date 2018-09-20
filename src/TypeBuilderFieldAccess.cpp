#include "TypeBuilderFieldAccess.h"

#include "AST.h"
#include "SymbolTable.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, ASTNode& node) {
    const auto symbolObject = symbols[node.asString()];
    if (symbolObject == nullptr) {
        throw std::runtime_error("trying to dereference an unknown symbol : " + node.asString());
    }
    
    const auto fieldAccessed = node[0].asString();
    const auto symbolField = (*symbolObject)[fieldAccessed];
    if (symbolField == nullptr) {
        throw std::runtime_error("trying to access to an undeclared field : " + fieldAccessed + " of " + node.asString() + (symbolObject->empty() ? (" of type " + symbolObject->getType().asString()) :  " of type " + (*symbolObject)[0].asString()));
    }
    return (*symbolObject)[0];

}
