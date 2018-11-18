#include "LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
    auto functionType = Type{ ExpressionType::FUNCTION };
    for (auto& paramNode : node[0]) {
        auto varType = paramNode->type().value();
        if(varType == ExpressionType::OBJECT) {
            varType.name(node.asString());
        }
        functionType.add(std::move(varType));
    }
	functionType.add(node[1].type().value());
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>) << "function declaration \""<< node.asString() <<"\" with type "<< functionType.asString();

    return functionType;
}
