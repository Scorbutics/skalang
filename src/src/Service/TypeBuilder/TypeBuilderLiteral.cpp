#include "Config/LoggerConfigLang.h"
#include "TypeBuilderLiteral.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::LITERAL>);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::UNARY>);

ska::Type ska::TypeBuilderOperator<ska::Operator::LITERAL>::build(const Script& script, OperateOn node) {
	auto& symbols = script.symbols();
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}

ska::Type ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const Script& script, OperateOn node) {
	auto& symbols = script.symbols();
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}
