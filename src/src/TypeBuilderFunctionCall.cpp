#include "LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const SymbolTable& symbols, const ASTNode& node) {
    const auto& functionIdentifier = node[0];
    const auto& type = node[0].type().value();
    const auto functionName = functionIdentifier.name();
    auto* symbol = symbols[functionName];
    auto* n = &node[0];
	auto* currentSymbolTable = symbols.current();//symbol->symbolTable();
    while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
        n = &(*n)[0];
        currentSymbolTable = currentSymbolTable->children()[0].get();
        const auto& fieldName = n->name();
        auto* fieldSymbol = (*currentSymbolTable)[fieldName];
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
        std::cout << "field symbol : " << (fieldSymbol != nullptr ? fieldSymbol->getType().asString() : "null") << std::endl;
#endif
        symbol = fieldSymbol;
    }
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    if(symbol != nullptr) {
        std::cout << "function call with type : " << symbol->getType().asString() << std::endl;
    }
#endif
    if(symbol == nullptr || symbol->empty() || (*symbol)[0] == ExpressionType::VOID) {
		return Type{ ExpressionType::VOID };
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>) << "returning type : " << symbol->getType().compound().back() << " of " << symbol->getType();
    return symbol->getType().compound().back();

}
