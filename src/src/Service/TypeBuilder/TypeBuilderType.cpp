#include "TypeBuilderType.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::TYPE>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::TYPE>::build(ScriptAST& script, OperateOn node) {
	auto result = Type{};
	const auto isBuiltIn = node.IsBuiltIn();
    if (!isBuiltIn) {
		const auto symbolType = node.GetSymbolType(script.symbols());
		if(symbolType.has_value()) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node looked \"" << symbolType.value() << "\" for node \"" << node.GetName() << "\"";
		} else {
			throw std::runtime_error("undeclared custom type \"" + node.GetName() + "\" (when trying to look on token type \"" + node.GetTypeName() + "\")");
		}
		result = node.IsObject() ? Type::MakeCustom<ExpressionType::OBJECT>(symbolType.value()) : symbolType.value();
   } else { 
		assert(!node.IsObject());
	   result = ExpressionTypeMap.at(node.GetName());
	   SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node is a built-in : \"" << node.GetName() << "\"";
   }

	//handles array type
	if (node.IsArray()) {
		result = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(result);
	}
		
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Resulting type : \"" << result << "\"";

	return result;
}
