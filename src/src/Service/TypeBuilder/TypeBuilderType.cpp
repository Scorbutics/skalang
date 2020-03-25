#include "TypeBuilderType.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::TYPE>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::TYPE>::build(const ScriptAST& script, OperateOn node) {
	auto result = Type{};
	const Symbol* resultSymbol = nullptr;
	const auto isBuiltIn = node.IsBuiltIn();
    if (!isBuiltIn) {
		auto* typeSymbol = node.GetTypeSymbol(script.symbols());
		if (typeSymbol != nullptr) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node looked \"" << typeSymbol->type() << "\" for node \"" << node.GetName() << "\"";
		} else {
			throw std::runtime_error("undeclared custom type \"" + node.GetName() + "\" (when trying to look on token type \"" + node.GetTypeName() + "\")");
		}

		resultSymbol = typeSymbol;
		if(node.IsObject()) {
			result = Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
		} else {
			result = typeSymbol->type();
		}

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

	return TypeHierarchy{result, resultSymbol};
}
