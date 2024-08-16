#include "Config/LoggerConfigLang.h"
#include <fstream>
#include "TypeBuilderUserDefinedObject.h"

#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::RETURN>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const ScriptAST& script, OperateOn node) {
	auto& previouslyDefinedSymbol = *script.symbols().current().back();
	const auto isScriptObject = previouslyDefinedSymbol.symbol() == nullptr && previouslyDefinedSymbol.symbol()->type() == ExpressionType::VOID && &script.symbols().current() == &script.symbols().current().parent();
	assert(previouslyDefinedSymbol.symbol() == nullptr || previouslyDefinedSymbol.symbol()->type() == ExpressionType::FUNCTION || isScriptObject);
	auto& resultSymbolTable = isScriptObject ? *script.symbols().enclosingType() : previouslyDefinedSymbol;
	return { ska::Type::MakeCustom<ExpressionType::OBJECT>(&resultSymbolTable), resultSymbolTable };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::RETURN>::build(const ScriptAST& script, OperateOn node) {
	return { node.GetValue().type().value(), &node.GetValue() };
}
