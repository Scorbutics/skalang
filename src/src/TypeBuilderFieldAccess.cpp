#include "TypeBuilderFieldAccess.h"

#include "AST.h"
#include "SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, const ASTNode& node) {
    const auto symbolObject = symbols[node.asString()];
    if (symbolObject == nullptr) {
        throw std::runtime_error("trying to dereference an unknown symbol : " + node.asString());
    }
    
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Symbol type : " << symbolObject->getType().asString();

    const auto fieldAccessed = node[0].asString();
	const auto& nestedFirst = symbolObject->symbolTable()->children();
	assert(nestedFirst.size() > 0);
	assert(nestedFirst[0]->children().size() > 0);
	const auto& table = (nestedFirst[0])->children()[0];
    const auto symbolField = (*table)[fieldAccessed];
    if (symbolField == nullptr) {
        throw std::runtime_error("trying to access to an undeclared field : " + fieldAccessed + " of " + node.asString() + (symbolObject->empty() ? (" of type " + symbolObject->getType().asString()) :  " of type " + (*symbolObject)[0].asString()));
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << fieldAccessed << " of type " << symbolField->getType().asString();

    return symbolField->getType();

}
