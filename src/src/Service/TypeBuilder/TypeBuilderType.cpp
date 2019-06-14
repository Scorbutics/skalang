#include "TypeBuilderType.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::TYPE>)

ska::Type ska::TypeBuilderOperator<ska::Operator::TYPE>::build(const Script& script, OperateOn node) {
	auto result = Type{};
	const auto isBuiltIn = node.IsBuiltIn();
    if (!isBuiltIn) {
		const Symbol* symbolType = node.GetSymbol(script.symbols());
		SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node looked \"" << (symbolType != nullptr ? symbolType->getType() : Type{}) << "\"";
		if (symbolType == nullptr) {
			throw std::runtime_error("undeclared custom type \"" + node.GetName() + "\" (when trying to look on token type \"" + node.GetTypeName() + "\")");
		}
		
		result = node.IsObject() ? Type::MakeCustom<ExpressionType::OBJECT>(symbolType) : symbolType->getType();
   } else { 
		assert(!node.IsObject());
	   result = ExpressionTypeMap.at(node.GetName());
	   SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node is a built-in : \"" << node.GetName() << "\"";
   }

	//handles array type
	if (node.IsArray()) {
		result = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(result);
	}
		
	return result;
}
