#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(ScriptAST& script, OperateOn node) {
	const auto typeObject = node.GetObjectType();
	if (typeObject != ExpressionType::OBJECT) {
		auto error = std::stringstream {};
		if (typeObject == ExpressionType::VOID) {
			error << "the class symbol table \"" << node.GetObjectNameNode().name() << "\" is not registered. Maybe you're trying to use the type you're defining in its definition...";
		} else {
			error << "the variable \"" << node.GetObjectNameNode().name() << "\" is not registered as an object but as a \"" << typeObject << "\"";
		}
		throw std::runtime_error(error.str());
	}

	const auto& fieldName = node.GetFieldNameNode().name();
	const auto symbolFieldType = typeObject.lookup(fieldName);
	if (!symbolFieldType.has_value()) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed \"" << fieldName << "\" (type \"" << symbolFieldType.value() << "\") of \"" << node.GetObjectNameNode().name() << "\"";
	
	if (symbolFieldType.value() == ExpressionType::VOID) {
		auto ss = std::stringstream{};
		ss << "field \"" << node.GetFieldNameNode().name() << "\" of \"" << node.GetObjectNameNode().name() << "\" has a void type, which is invalid";
		throw std::runtime_error(ss.str());
	}

	return symbolFieldType.value();
}
