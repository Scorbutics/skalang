#include "Config/LoggerConfigLang.h"
#include "TypeBuilderLiteral.h"

#include "TypeBuilderCalculatorDispatcher.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::LITERAL>);
SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::UNARY>);

ska::Type ska::TypeBuilderOperator<ska::Operator::LITERAL>::build(const SymbolTable& symbols, const ASTNode& node) {
	return TypeBuilderBuildFromTokenType(symbols, node);
}

ska::Type ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const SymbolTable& symbols, const ASTNode& node) {
	return TypeBuilderBuildFromTokenType(symbols, node);
}
