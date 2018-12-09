#include "LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
	auto functionName = node.name();
	assert(!symbols.nested().empty());
	const auto& symbolTableNested = *symbols.nested().back();
	const auto symbol = symbolTableNested(functionName);
	assert(symbol != nullptr);
	return symbol->getType();
}
