
#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	assert(m_scopedTable != nullptr);
	
	if (!m_scopedTable->children().empty()) {
		const auto& innerSymbolTable = m_scopedTable->children().back();
		if(!innerSymbolTable->children().empty()) {
			for(auto& s: innerSymbolTable->children()) {
				for(auto& ss : (*s)) {
					SLOG(ska::LogLevel::Error) << "field " << ss->getName();
				}
			}
			const auto& st = *innerSymbolTable->children()[innerSymbolTable->children().size() - 1];
			return st[fieldSymbolName];
		}
	}

	SLOG(ska::LogLevel::Error) << "UNABLE TO FIND " << fieldSymbolName;
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](fieldSymbolName));
}

std::size_t ska::Symbol::size() const {
    return m_category.compound().size();
}