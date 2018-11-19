#include "LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
	auto functionName = node.asString();
	assert(!symbols.nested().empty());
	auto functionType = Type{ functionName, *symbols.nested().back() };
    for (auto& paramNode : node[0]) {
        functionType.add(paramNode->type().value());
    }
	auto returnType = node[1].type().value();
	functionType.add(returnType == ExpressionType::OBJECT ? Type{ functionName, ExpressionType::OBJECT } : returnType);
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>) << "function declaration \""<< node.asString() <<"\" with type "<< functionType.asString();

    return functionType;
}
