#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
	auto functionName = node.name();
	auto functionType = Type{ functionName, ExpressionType::FUNCTION, *symbols.current() };
    for (auto& paramNode : node) {
		auto type = paramNode->type().value() == ExpressionType::OBJECT ? Type{ functionName, ExpressionType::OBJECT, *functionType.userDefinedSymbolTable() } : paramNode->type().value();
        functionType.add(type);
    }
	
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration \""<< node <<"\" with type "<< functionType;

    return functionType;
}
