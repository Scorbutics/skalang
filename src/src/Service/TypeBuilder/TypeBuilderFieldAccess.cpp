#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, OperateOn node) {
	auto objectType = node.GetObjectType(symbols);
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Symbol type : " << objectType;
	if (objectType.userDefinedSymbolTable() == nullptr) {
		if(node.GetObjectTypeName().empty() {
			throw std::runtime_error("trying to dereference an unknown symbol : " + GetObjectName());
		}
		throw std::runtime_error("the class symbol table " + node.GetFieldName() + " is not registered. Maybe you're trying to use the type you're defining in its definition...");
	}
	
	const auto symbolField = node.GetFieldSymbolFromObjectSymbolTable(objectType);	
    if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : " << fieldAccessed << " of " << node.GetObjectName() << " of type " << objectType;
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << fieldAccessed << " of type " << symbolField->getType();

	return Type::isNamed(symbolField->getType()) ? Type{ symbolField->getType(), *table } : symbolField->getType();

}
