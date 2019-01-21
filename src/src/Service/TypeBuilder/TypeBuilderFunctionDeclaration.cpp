#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(StatementParser& parser, const SymbolTable& symbols, const ASTNode& node) {
	auto functionName = node.name();
	assert(!symbols.nested().empty());
	//const auto& symbolTableNested = *symbols.nested().back();
	const auto symbol = (*symbols.current())(functionName);
	assert(symbol != nullptr);
	return symbol->getType();
}
