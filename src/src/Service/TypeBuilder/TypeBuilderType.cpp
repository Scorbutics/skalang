#include "TypeBuilderType.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::TYPE>)

ska::Type ska::TypeBuilderOperator<ska::Operator::TYPE>::build(const ScriptAST& script, OperateOn node) {
	auto result = Type{};
	const auto isBuiltIn = node.IsBuiltIn();
    if (!isBuiltIn) {
		const Symbol* symbolType = node.GetSymbol(script.symbols());
		if(symbolType != nullptr) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node looked \"" << symbolType->type() << "\" for node \"" << node.GetName() << "\"";
		} else {
			throw std::runtime_error("undeclared custom type \"" + node.GetName() + "\" (when trying to look on token type \"" + node.GetTypeName() + "\")");
		}
		result = node.IsObject() ? Type::MakeCustom<ExpressionType::OBJECT>(symbolType) : symbolType->type();
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
