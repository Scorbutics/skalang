#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const SymbolTable& symbols, ASTNode& node) {
    const auto& functionIdentifier = node[0];
    const auto& type = TypeBuilderDispatchCalculation(symbols, node[0]);
    const auto functionName = functionIdentifier.asString();
    auto* symbol = symbols[functionName];
    auto* n = &node[0];
	auto* currentSymbolTable = symbols.current();//symbol->symbolTable();
    while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
        n = &(*n)[0];
        currentSymbolTable = currentSymbolTable->children()[0].get();
        const auto& fieldName = n->asString();
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
        return ExpressionType::VOID;
    }

    //std::cout << "returning type : " << symbol->category.compound().back().asString() << " of " << symbol->category.asString() << std::endl;
    return symbol->getType().compound().back();

}
