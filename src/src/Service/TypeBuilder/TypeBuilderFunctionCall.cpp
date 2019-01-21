#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Operation/OperationFunctionDeclaration.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const SymbolTable& symbols, OperateOn node) {
    auto* symbol = node.GetFunctionReturnSymbol(symbols);

	SLOG(LogLevel::INFO) << "function call with type : " << ((symbol != nullptr) ? symbol->getType().asString() : "");    

    if(symbol == nullptr) {
		return Type{ ExpressionType::VOID };
	} else if (symbol->getType() == ExpressionType::OBJECT) {
		auto* symbolComplexType = symbols[symbol->getName()];
		if (symbolComplexType == nullptr) {
			return Type{ ExpressionType::VOID };
		}
		return symbolComplexType->getType();
	} else if (symbol->empty() || (*symbol)[0] == ExpressionType::VOID) {
		return Type{ ExpressionType::VOID };
	}

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>) << "returning type : " << symbol->getType().compound().back() << " of " << symbol->getType();
    return symbol->getType().compound().back();
}
