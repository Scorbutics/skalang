#include "ScopedSymbolTable.h"
#include "Service/SymbolFieldResolver.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::ScopedSymbolTable)

ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}

const ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() const {
	return m_parent;
}

ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name) {
	SLOG(ska::LogLevel::Info) << "\tSymbol Created \"" << name << "\" with scoped table";
	return emplace(Symbol{ m_children.size(), name, SymbolFieldResolver{this} });
}

ska::Symbol& ska::ScopedSymbolTable::emplace(Symbol symbol) {
	auto name = symbol.name();
	SLOG(ska::LogLevel::Debug) << "\tSymbol \"" << name << "\" \"" <<  symbol.type() << "\"";
	if(m_symbols.find(name) == m_symbols.end()) {
		m_symbols.emplace(name, std::move(std::move(symbol)));
	} else {
		throw std::runtime_error("Symbol already exists : " + name);
	}

	auto& s = m_symbols.at(name);
	SLOG(ska::LogLevel::Info) << "\tSymbol Inserted \"" << name << "\" \"" << s.type() << "\"";
	return s;
}

ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name, const ScriptAST& script) {
	SLOG(ska::LogLevel::Info) << "\tSymbol Created \"" << name << "\" with script \"" << script.name() << "\"";
	return emplace(Symbol{ m_children.size(), name, SymbolFieldResolver{script.handle()} });
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested(Symbol* s) {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	lastChild.m_parentSymbol = s;
	
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
