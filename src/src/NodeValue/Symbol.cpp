#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Symbol)

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	return m_data.lookup(m_tableIndex, fieldSymbolName);
}

std::size_t ska::Symbol::size() const {
	return m_category.compound().size();
}

ska::Symbol& ska::Symbol::operator=(const Symbol& s) {
	m_data = s.m_data;
	m_name = s.m_name;
	m_tableIndex = s.m_tableIndex;
	m_category = s.m_category;
	SLOG(ska::LogLevel::Debug) << "   Copy, Symbol " << s.name() << " " << s.m_category << " copied to " << m_name << " " << m_category;
	return *this;
}

ska::Symbol& ska::Symbol::operator=(Symbol&& s) noexcept {
	m_data = std::move(s.m_data);
	m_name = std::move(s.m_name);
	m_tableIndex = std::move(s.m_tableIndex);
	m_category = std::move(s.m_category);
	SLOG(ska::LogLevel::Debug) << "   Move, Symbol " << s.name() << " " << s.m_category << " moved to " << m_name << " " << m_category;
	return *this;
}

bool ska::Symbol::operator==(const Symbol& sym) const {
	const auto compareData = m_data == sym.m_data;
	return m_name == sym.m_name && 
		m_tableIndex == sym.m_tableIndex && /*
		m_category== sym.m_category */
	compareData;
}

bool ska::Symbol::changeTypeIfRequired(const Type& type) {
	if (/*m_master == this && */m_category != type) {
		m_category = type;
		return true;
	}
	return false;
}

void ska::Symbol::implement(Symbol& symbol) {
	if (symbol.m_master == this) {
		return;
	}

	symbol.m_master->m_implementationReferences.erase(&symbol);	
	symbol.m_master = this;
	m_implementationReferences.insert(&symbol);
}
