#include "SymbolTypeAccess.h"
#include "Type.h"

const ska::Symbol* ska::TypeSymbolAccess(const Type& type) {
	return type.m_symbol;
}