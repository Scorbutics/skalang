#include "OperationTypeFunctionCall.h"
#include "Service/SymbolTable.h"

const ska::Symbol* ska::OperationType<ska::Operator::FUNCTION_CALL>::GetFunctionReturnSymbol(const SymbolTable& symbols) {
	const Symbol* symbol = symbols[GetFunctionName()];
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