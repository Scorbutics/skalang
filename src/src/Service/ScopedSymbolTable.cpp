#include "Config/LoggerConfigLang.h"
#include "ScopedSymbolTable.h"
#include "Service/SymbolFieldResolver.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScopedSymbolTable)

ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}

const ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() const {
	return m_parent;
}

ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name) {
	SLOG(ska::LogLevel::Info) << "\tSymbol Created \"" << name << "\" with scoped table";
	return emplace(make(m_children.size(), std::move(name), *this));
}

ska::Symbol& ska::ScopedSymbolTable::emplace(Symbol symbol) {
	auto name = symbol.name();
	SLOG(ska::LogLevel::Debug) << "\tSymbol \"" << name << "\" \"" <<  symbol.type() << "\"";
	if(m_symbols.find(name) == m_symbols.end()) {
		m_symbols.emplace(name, std::make_unique<Symbol>(std::move(symbol)));
	} else {
		throw std::runtime_error("Symbol already exists : " + name);
	}

	auto& s = m_symbols.at(name);
	SLOG(ska::LogLevel::Info) << "\tSymbol Inserted \"" << name << "\" \"" << s.type() << "\"";
	return s;
}

ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name, const ScriptAST& script) {
	SLOG(ska::LogLevel::Info) << "\tSymbol Created \"" << name << "\" with script \"" << script.name() << "\"";
	return emplace(make(m_children.size(), name, *script.handle()));
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested(Symbol* s, bool isExported) {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	lastChild.m_parentSymbol = s;
	lastChild.m_exported = isExported;
	
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}

bool ska::ScopedSymbolTable::changeTypeIfRequired(const std::string& symbolName, const Type& value) {
	auto* symbol = (*this)[symbolName];
	if (symbol == nullptr) {
		auto ss = std::stringstream{};
		ss << "bad symbol \"" << symbolName << "\" : cannot assign type \"" << value << "\"";
		throw std::runtime_error(ss.str());
	}
	return symbol->changeTypeIfRequired(value);
}

const ska::Symbol* ska::ScopedSymbolTable::operator[](const std::string& key) const {
	const auto valueIt = m_symbols.find(key);
	if (valueIt == m_symbols.end()) {
		return &m_parent == this ? nullptr : m_parent[key];
	}
	return valueIt == m_symbols.end() || *valueIt == nullptr ? nullptr : (*valueIt).get();
}

ska::Symbol* ska::ScopedSymbolTable::operator[](const std::string& key) {
	auto valueIt = m_symbols.find(key);
	if (valueIt == m_symbols.end()) {
		return &m_parent == this ? nullptr : m_parent[key];
	}
	return valueIt == m_symbols.end() || *valueIt == nullptr ? nullptr : (*valueIt).get();
}

const ska::Symbol* ska::ScopedSymbolTable::operator[](std::size_t index) const {
	return index < m_symbols.size() ? &m_symbols.at(index) : nullptr;
}

ska::Symbol* ska::ScopedSymbolTable::operator[](std::size_t index) {
	return index < m_symbols.size() ? &m_symbols.at(index) : nullptr;
}

const ska::Symbol* ska::ScopedSymbolTable::operator()(const std::string& key) const {
	const auto valueIt = m_symbols.find(key);
	return valueIt == m_symbols.end() || *valueIt == nullptr ? nullptr : (*valueIt).get();
}

ska::Symbol* ska::ScopedSymbolTable::operator()(const std::string& key) {
	auto valueIt = m_symbols.find(key);
	return valueIt == m_symbols.end() || *valueIt == nullptr ? nullptr : (*valueIt).get();
}

std::optional<std::size_t> ska::ScopedSymbolTable::id(const Symbol& field) const {
	if (m_symbols.atOrNull(field.name()) == nullptr || &m_symbols.at(field.name()) != &field) {
		return {};
	}

	return m_symbols.id(field.name());
}
