#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Operation/OperationFunctionDeclaration.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const SymbolTable& symbols, OperateOn node) {
/* auto* symbol = node.GetFunctionReturnSymbol(symbols);
*/

	const auto type = node.GetFunctionType();
	SLOG(LogLevel::Info) << "function " << type.getName() << " call with type : " << type;
    
    /*
    auto* n = &node[0];
	auto* currentSymbolTable = symbols.current();
	while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
		n = &(*n)[0];
		currentSymbolTable = currentSymbolTable->children()[0].get();
		const auto& fieldName = n->name();
		symbol = (*currentSymbolTable)[fieldName];
	}
    */

    if(type.compound().size() == 0) {
        throw std::runtime_error("Unable to get function return type");
    }

	return type.compound().back();
/*
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
*/
}
