#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	return m_data[fieldSymbolName];
}

ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](fieldSymbolName));
}

std::size_t ska::Symbol::size() const {
	return m_category.compound().size();
}

bool ska::Symbol::operator==(const Symbol& sym) const {
	const auto compareData = m_data == sym.m_data;
	return m_name == sym.m_name && /*
		m_category== sym.m_category */
	compareData;
}

void ska::Symbol::forceType(Type t) {
	m_category = t;
	//m_category.m_symbol = this;
}
