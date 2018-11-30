#include "Symbol.h"
#include "ScopedSymbolTable.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& symbol) const {
	assert(m_scopedTable != nullptr);
	if (!m_scopedTable->children().empty()) {
		const auto& st = *m_scopedTable->children()[0];
		return st[symbol];
	}
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& symbol) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](symbol));
}

std::size_t ska::Symbol::size() const {
    return m_category.compound().size();
}