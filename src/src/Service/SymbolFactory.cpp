#include "SymbolFactory.h"

// TODO simplify ?

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScopedSymbolTable& table) {
	return { tableIndex, name };
}

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScriptHandleAST& script) {
	return { tableIndex, name };
}
