#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, OperateOn node) {
	const auto& typeObject = node.GetObjectType();
	if (typeObject == ExpressionType::VOID) {
        for(auto& s : typeObject.compound()) {
            SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << s;
        }
		throw std::runtime_error("the class symbol table " + node.GetObjectName() + " is not registered. Maybe you're trying to use the type you're defining in its definition...");
	}

	const auto& fieldName = node.GetFieldName();
	const auto& symbolField = typeObject[fieldName];
    if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : " << node.GetFieldName() << " of " << node.GetObjectName();
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << node.GetFieldName() << " (type " << symbolField->getType() << ") of " << node.GetObjectName();
	return symbolField->getType();
}
