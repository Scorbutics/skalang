#include "ScopedSymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScopedSymbolTable)

ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}

ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name) {
    
    {
		auto symbol = Symbol{ name, *this, ska::Type{} };
		SLOG(ska::LogLevel::Debug) << "\tSymbol \"" << name << "\" \"" <<  symbol.getType() << "\"";
        if(m_symbols.find(name) == m_symbols.end()) {
            m_symbols.emplace(name, std::move(symbol));
        } else {
			throw std::runtime_error("Symbol already exists : " + name);
        }
    }

    auto& s = m_symbols.at(name);
	SLOG(ska::LogLevel::Debug) << "\tSymbol Inserted \"" << name << "\" \"" << s.getType() << "\"";
    return s;
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}
