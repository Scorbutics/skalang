#include "Config/LoggerConfigLang.h"
#include "ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScopedSymbolTable)

const std::string ska::ScopedSymbolTable::EMPTY_STR = "";

ska::ScopedSymbolTable::ScopedSymbolTable(std::string rootName):
	m_symbol(make(0, std::move(rootName), *this)) {
	//m_symbol.value().changeTypeIfRequired(Type::MakeCustom<ska::ExpressionType::FUNCTION>(this));
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}

const ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() const {
	return m_parent;
}

void ska::ScopedSymbolTable::ensureNotLocked() const {
	if (m_locked) {
		auto ss = std::stringstream {};
		ss << "Symbol \"";
		if (m_symbol.has_value()) {
			ss << m_symbol.value();
		} else {
			ss << "(null)";
		}
		ss << "\" is locked";
		throw std::runtime_error(ss.str());
	}
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested(std::string name, const ScriptAST* script) {
	ensureNotLocked();
	if (script != nullptr) {
		auto& table = createNested(std::make_optional(make(m_children.size(), std::move(name), script->handle()->symbols().root())));
		//table.changeTypeIfRequired(script->handle()->symbols().root().symbol()->type());
		return table;
	}
	return createNested(name.empty() ? std::optional<Symbol>{} : std::make_optional(make(m_children.size(), name, *this)));
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested(std::optional<Symbol> optSymbol) {
	ensureNotLocked();
	if (!optSymbol.has_value()) {
		// Create an unnamed nested symbol table (in case of blocks of code for example)
		// Generate a name which is in fact an id (the index)
		m_children.emplace(std::to_string(m_children.size()), std::make_unique<ska::ScopedSymbolTable>(*this));
		return m_children.back();
	}
	auto symbol = std::move(optSymbol.value());

	if(m_children.find(symbol.name()) != m_children.end()) {
		throw std::runtime_error("Symbol already exists : " + symbol.name());
	}

	SLOG(ska::LogLevel::Debug) << "\tSymbol created \"" << symbol.name() << "\"";

	// Need to copy the name before symbol is moved
	const auto name = symbol.name();
	auto table = std::make_unique<ska::ScopedSymbolTable>(*this, std::move(symbol));

	// Preserve the future newly created symbol table in a reference in order to return it
	auto& result = *table;

	// Now we can move the owner pointer
	m_children.emplace(name, std::move(table));

	SLOG(ska::LogLevel::Info) << "\tSymbol inserted \"" << name << "\" \"" << result.m_symbol.value().type() << "\"";
	return result;
}

bool ska::ScopedSymbolTable::changeTypeIfRequired(const Type& value) {
	ensureNotLocked();
	if (!m_symbol.has_value()) {
		auto ss = std::stringstream{};
		ss << "bad symbol: cannot assign type \"" << value << "\"";
		throw std::runtime_error(ss.str());
	}
	return m_symbol.value().changeTypeIfRequired(value);
}

template<class Children, class Table>
static Table* HierarchicalLookup(Table* classTable, const Table& current, Children& children, Table& parent, const std::string& key) {
	// First, direct lookup in parent class table, if it exists
	if (classTable != nullptr) {
		auto result = (*classTable)(key);
		if (result != nullptr) {
			return result;
		}
	}

	// Then look into direct children symbols
	auto valueIt = children.find(key);
	if (valueIt == children.end()) {
		// If still not found, look into parent table
		return &parent == &current ? nullptr : parent[key];
	}
	return *valueIt == nullptr ? nullptr : (*valueIt).get();
}

template<class Children, class Table>
static Table* DirectLookup(Table* classTable, Children& children, const std::string& key) {
	// First, direct lookup in parent class table, if it exists
	if (classTable != nullptr) {
		auto result = (*classTable)(key);
		if (result != nullptr) {
			return result;
		}
	}

	// Then look into direct children symbols
	auto valueIt = children.find(key);
	return valueIt == children.end() || *valueIt == nullptr ? nullptr : (*valueIt).get();
}

const ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator[](const std::string& key) const {
	return HierarchicalLookup(m_classTable, *this, m_children, m_parent, key);
}

ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator[](const std::string& key) {
	return HierarchicalLookup(m_classTable, *this, m_children, m_parent, key);
}

template<class Children, class Table>
static Table* IndexLookup(Table* classTable, Children& children, std::size_t index) {
	if (classTable != nullptr) {
		if (index < classTable->size()) {
			return (*classTable)[index];
		}
		index -= classTable->size();
	}
	return index < children.size() ? &children.at(index) : nullptr;
}

const ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator[](std::size_t index) const {
	return IndexLookup<const decltype(m_children), const ScopedSymbolTable>(m_classTable, m_children, index);
}

ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator[](std::size_t index) {
	return IndexLookup(m_classTable, m_children, index);
}

const ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator()(const std::string& key) const {
	return DirectLookup(m_classTable, m_children, key);
}

ska::ScopedSymbolTable* ska::ScopedSymbolTable::operator()(const std::string& key) {
	return DirectLookup(m_classTable, m_children, key);
}

std::optional<std::size_t> ska::ScopedSymbolTable::id(const Symbol& field) const {
	std::optional<std::size_t> parentId = m_classTable != nullptr ? m_classTable->id(field) : std::optional<std::size_t>{};
	if (m_children.atOrNull(field.name()) == nullptr || m_children.at(field.name()).symbol() != &field) {
		return parentId;
	}

	const auto childId = m_children.id(field.name());
	return (parentId.has_value() ? parentId.value() : 0) + childId;
}

void ska::ScopedSymbolTable::implement(ScopedSymbolTable& childInstanceSymbolTable) {
	if (!m_symbol.has_value() || !childInstanceSymbolTable.m_symbol.has_value()) {
		throw std::runtime_error("cannot implement data from a class symbol table without having any current symbol");
	}

	if (!childInstanceSymbolTable.m_children.empty()) {
		auto ss = std::stringstream {};
		ss << "current child symbol \"" << childInstanceSymbolTable.m_symbol.value() <<
			"\" already has children, so it cannot implement the current symbol \"" << m_symbol.value() << "\"";
		throw std::runtime_error(ss.str());
	}

	if (childInstanceSymbolTable.m_classTable == this) {
		return;
	}
	SLOG(ska::LogLevel::Info) << "Implementing class symbol table " << m_symbol.value() << " into " << *childInstanceSymbolTable.m_classTable;

	childInstanceSymbolTable.m_classTable = this;
	m_locked = true;
}
