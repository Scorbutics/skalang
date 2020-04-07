#include "Config/LoggerConfigLang.h"
#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Symbol)

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::Symbol)

ska::Symbol::Symbol(std::size_t tableIndex, std::string name, SymbolFieldResolver fields) :
	m_master(this),
	m_name(std::move(name)),
	m_data(std::move(fields)),
	m_tableIndex(tableIndex) {
}

template <class Return, class Data, class Master>
static Return Lookup(Data& data, Master* master, const std::string& fieldSymbolName) {
	auto* table = data.lookup();
	if (table != nullptr) {
		auto* found = (*table)(fieldSymbolName);
		if (found != nullptr) {
			return found;
		}
	}

	if(master != nullptr) {
		LOG_INFO << "Symbol \"" << fieldSymbolName << "\" not found here, looking into master symbol \"" << master->name() << "\"";
		return (*master)(fieldSymbolName);
	}
	return nullptr;
}

template <class Return, class Data, class Master>
static Return Lookup(Data& data, Master* master, std::size_t index) {
	auto* table = data.lookup();
	if (table != nullptr) {
		auto* found = (*table)[index];
		if (found != nullptr) {
			return found;
		}
		LOG_INFO << "Symbol index \"" << index << "\" not found here";
		if (table->size() > index) {
			index -= table->size();
		}
	} else {
		LOG_INFO << "Symbol index \"" << index << "\" not found here";
	}

	if (master != nullptr) {
		LOG_INFO << "looking into master symbol \"" << master->name() << "\"";
		return (*master)[index];
	}
	return nullptr;
}

std::size_t ska::Symbol::size() const {
	auto* table = m_data.lookup();
	return table == nullptr ? 0 : table->size();
}

ska::Symbol::Symbol(const Symbol& s) {
	*this = s;
}

ska::Symbol::Symbol(Symbol&& s) noexcept {
	*this = std::move(s);
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
	if (!compareData) {
		return m_master != this && (*m_master) == sym || sym.m_master != &sym && (*sym.m_master) == *this;
	}

	return m_name == sym.m_name &&
		m_tableIndex == sym.m_tableIndex;
}

bool ska::Symbol::changeTypeIfRequired(const Type& type) {
	if (m_category == ExpressionType::VOID || m_category.type() == type.type()) {
		m_category = type;
		return true;
	}

	return m_category.tryChangeSymbol(type);
}

void ska::Symbol::implement(Symbol& symbol) {
	if (symbol.m_master == this || &symbol == this) {
		return;
	}
	SLOG(ska::LogLevel::Info) << "Implementing symbol " << symbol << " into " << m_name << " type (" << m_category << ")";

	symbol.m_master->m_implementationReferences.erase(&symbol);	
	symbol.m_master = this;
	m_implementationReferences.insert(&symbol);
}

const ska::Symbol* ska::Symbol::operator[](std::size_t index) const {
	return Lookup<const Symbol*>(m_data, m_master != this ? m_master : nullptr, index);
}

ska::Symbol* ska::Symbol::operator[](std::size_t index) {
	return Lookup<Symbol*>(m_data, m_master != this ? m_master : nullptr, index);
}

const ska::Symbol* ska::Symbol::operator()(const std::string& symbolName) const {
	return Lookup<const Symbol*>(m_data, m_master != this ? m_master : nullptr, symbolName);
}

ska::Symbol* ska::Symbol::operator()(const std::string& symbolName) {
	return Lookup<Symbol*>(m_data, m_master != this ? m_master : nullptr, symbolName);
}

const ska::Symbol* ska::Symbol::back() const {
	auto* table = m_data.lookup();
	return table == nullptr ? nullptr : table->back();
}

ska::Symbol* ska::Symbol::back() {
	auto* table = m_data.lookup();
	return table == nullptr ? nullptr : table->back();
}

std::ostream& ska::operator<<(std::ostream& stream, const Symbol& symbol) {
	stream << symbol.m_name << " (" << symbol.m_category << ")";
	return stream;
}

bool ska::Symbol::empty() const {
	return size() == 0;
}

void ska::Symbol::open() {
	m_closed = false;
	m_data.open();
}

void ska::Symbol::close() {
	m_closed = true; 
	m_data.close();
}
