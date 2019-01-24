#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const SymbolTable& symbols, OperateOn node) {
	const auto* symbolObject = symbols[node.GetObjectTypeName()];
	if (symbolObject == nullptr) {
		if(node.GetObjectTypeName().empty()) {
			throw std::runtime_error("trying to dereference an unknown symbol : " + node.GetObjectName());
		}
		throw std::runtime_error("the class symbol table " + node.GetFieldName() + " is not registered. Maybe you're trying to use the type you're defining in its definition...");
	}

	const auto* symbolField = (*symbolObject)[node.GetFieldName()];
    if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : " << node.GetFieldName() << " of " << node.GetObjectName() << " (type " << node.GetFieldTypeName() << ")";
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << node.GetFieldName() << " (type " << symbolField->getType() << ") of " << node.GetObjectName();
	return symbolField->getType();
}
