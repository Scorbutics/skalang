#include "OperationFunctionCall.h"
#include "Service/SymbolTable.h"

ska::Symbol* ska::Operation<Operator::FUNCTION_CALL>::GetFunctionReturnSymbol(const SymbolTable& symbols) {
	auto* symbol = symbols[functionName];
	auto* n = &node[0];
	auto* currentSymbolTable = symbols.current();
	while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
		n = &(*n)[0];
		currentSymbolTable = currentSymbolTable->children()[0].get();
		const auto& fieldName = n->name();
		symbol = (*currentSymbolTable)[fieldName];
	}
	return symbol;
 }