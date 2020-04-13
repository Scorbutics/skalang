#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const ScriptAST& script, OperateOn node) {
	const auto typeObject = node.GetObjectType();

	const auto& fieldName = node.GetFieldNameNode().name();

	// Handling built-in direct fields
	if (typeObject == ExpressionType::ARRAY) {
		if (fieldName == "size") {
			return Type::MakeBuiltIn<ExpressionType::INT>();
		}
		auto ss = std::stringstream{};
		ss << "trying to access an undeclared built-in field \"" << fieldName << "\" of the type \"" << typeObject << "\"";
		throw std::runtime_error(ss.str());
	}

	const auto* symbolObject = node.GetObjectSymbol();

	// Handling custom objects
	if (symbolObject == nullptr || typeObject != ExpressionType::OBJECT) {
		auto error = std::stringstream {};
		if (typeObject == ExpressionType::VOID) {
			error << "the class symbol table \"" << node.GetObjectNameNode().name() << "\" is not registered. Maybe you're trying to use the type you're defining in its definition...";
		} else {
			error << "the variable \"" << node.GetObjectNameNode().name() << "\" is not registered as an object but as a \"" << typeObject << "\" type";
		}
		throw std::runtime_error(error.str());
	}

	const auto* symbolField = (*symbolObject)(fieldName);
	if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed \"" << fieldName << "\" (type \"" << symbolField->type() << "\") of \"" << node.GetObjectNameNode().name() << "\"";

	if (symbolField->type() == ExpressionType::VOID) {
		auto ss = std::stringstream{};
		ss << "field \"" << node.GetFieldNameNode().name() << "\" of \"" << node.GetObjectNameNode().name() << "\" has a void type, which is invalid";
		throw std::runtime_error(ss.str());
	}

	return *symbolField;
}
