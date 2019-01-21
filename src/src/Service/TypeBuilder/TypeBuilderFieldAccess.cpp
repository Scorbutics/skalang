#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(StatementParser& parser, const SymbolTable& symbols, const ASTNode& node) {
	assert(node[0].type() == ExpressionType::OBJECT && node.size() == 2);
	auto symbolObjectType = node[0].type().value();
    if (symbolObjectType.getName().empty() || symbolObjectType.userDefinedSymbolTable() == nullptr) {
        throw std::runtime_error("trying to dereference an unknown symbol : " + node[0].name());
    }
	auto symbolObject = symbols[symbolObjectType.getName()];
	if (symbolObject == nullptr && symbolObjectType.userDefinedSymbolTable() == nullptr) {
		throw std::runtime_error("trying to use an unknown type : " + symbolObjectType.getName());
	}
	if (symbolObject != nullptr) {
		symbolObjectType = symbolObject->getType();
	}
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Symbol type : " << symbolObjectType;

    const auto fieldAccessed = node[1].name();
	assert(!fieldAccessed.empty());
	if (symbolObjectType.userDefinedSymbolTable() == nullptr) {
		throw std::runtime_error("the class symbol table " + fieldAccessed + " is not registered. Maybe you're trying to use the type you're defining in its definition...");
	}
	const auto& userDefinedTable = symbolObjectType.userDefinedSymbolTable()->children();
	assert(userDefinedTable.size() > 0);
	const auto& table = userDefinedTable.back();
    const auto symbolField = (*table)(fieldAccessed);
    if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : " << fieldAccessed << " of " << node[0].name() << " of type " << symbolObjectType;
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed " << fieldAccessed << " of type " << symbolField->getType();

	return Type::isNamed(symbolField->getType()) ? Type{ symbolField->getType(), *table } : symbolField->getType();

}
