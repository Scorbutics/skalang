#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
	auto& symbols = script.symbols();
	const auto symbol = symbols(functionName);
	assert(symbol != nullptr);
	return symbol->type();
}
