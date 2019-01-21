#include "Config/LoggerConfigLang.h"
#include "TypeBuilderLiteral.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Operation/OperationUnary.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::LITERAL>);
SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::UNARY>);

ska::Type ska::TypeBuilderOperator<ska::Operator::LITERAL>::build(const SymbolTable& symbols, OperateOn node) {
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}

ska::Type ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const SymbolTable& symbols, OperateOn node) {
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}
