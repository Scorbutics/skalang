#include "TypeBuilderFieldAccess.h"

#include "AST.h"
#include "SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, const ASTNode& node) {
    const auto symbolObject = symbols[node[0].name()];
    if (symbolObject == nullptr) {
        throw std::runtime_error("trying to dereference an unknown symbol : " + node[0].name());
    }
    
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Symbol type : " << symbolObject->getType();

    const auto fieldAccessed = node[1].name();
	const auto& userDefinedTable = symbolObject->symbolTable()->children();
	assert(userDefinedTable.size() > 0);
	assert(userDefinedTable[0]->children().size() > 0);
	const auto& table = (userDefinedTable[0])->children().back();
    const auto symbolField = (*table)[fieldAccessed];
    if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : " << fieldAccessed << " of " << node[0].name(); 
		symbolObject->empty() ? (ss << " of type " << symbolObject->getType()) : ss << " of type " << (*symbolObject)[0];
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << fieldAccessed << " of type " << symbolField->getType();

	return symbolField->getType();//Type{ symbolField->getType(), *table };

}
