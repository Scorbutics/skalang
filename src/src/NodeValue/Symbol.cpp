
#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	assert(m_scopedTable != nullptr);
	
	if (!m_scopedTable->children().empty()) {
		for(const auto& innerSymbolTable : m_scopedTable->children()) {
		    if(!innerSymbolTable->children().empty()) {
			    const auto& st = *innerSymbolTable->children().back();
			    const auto* result = st[fieldSymbolName];
                if(result != nullptr) {
                    return result;
                }
		    }
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
