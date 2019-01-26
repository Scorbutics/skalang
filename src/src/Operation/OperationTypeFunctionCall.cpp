#include "OperationTypeFunctionCall.h"
#include "Service/SymbolTable.h"

const ska::Type ska::OperationType<ska::Operator::FUNCTION_CALL>::GetFunctionReturnSymbol(const SymbolTable& symbols) {
	const auto& type = GetFunctionType();
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

	return type;
 }
