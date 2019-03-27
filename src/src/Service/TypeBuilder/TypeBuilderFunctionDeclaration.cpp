#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const Script& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
	auto& symbols = script.symbols();
	assert(!symbols.nested().empty());
	const auto symbol = (*symbols.current())(functionName);
	assert(symbol != nullptr);
	return symbol->getType();
}
