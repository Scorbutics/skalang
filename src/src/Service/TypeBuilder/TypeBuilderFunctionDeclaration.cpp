#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Operation/OperationFunctionDeclaration.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const SymbolTable& symbols, OperateOn node) {
	auto functionName = node.GetFunctionName();
	assert(!symbols.nested().empty());
	const auto symbol = (*symbols.current())(functionName);
	assert(symbol != nullptr);
	return symbol->getType();
}
