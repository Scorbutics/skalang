#include "SymbolFactory.h"

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScopedSymbolTable& table) {
	return { tableIndex, name, SymbolFieldResolver{ name, tableIndex, &table } };
}

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScriptHandleAST& script) {
	return { tableIndex, name, SymbolFieldResolver{ name, tableIndex, &script } };
}
