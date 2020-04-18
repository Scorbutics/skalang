#include "Config/LoggerConfigLang.h"
#include "NodeValue/Symbol.h"
#include "NodeValue/ScriptAST.h"
#include "ScopedSymbolTable.h"
#include "SymbolFieldResolver.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolFieldResolver)

ska::SymbolFieldResolver::SymbolFieldResolver(const std::string& name, std::size_t tableIndex, Variant value) :
	m_tableIndex(tableIndex),
	m_inputData(std::move(value)),
	m_symbolName(name) {

	if(std::holds_alternative<ScopedSymbolTable*>(m_inputData)) {
		SLOG(ska::LogLevel::Debug) << "Creating symbol fields resolver from table";
	} else {
		SLOG(ska::LogLevel::Debug) << "Creating symbol fields resolver from script";
	}
}

ska::SymbolFieldResolver::SymbolFieldResolver(SymbolFieldResolver&& toMove) {
	std::lock_guard lock{ toMove.m_cacheMutex };
	m_tableIndex = toMove.m_tableIndex;
	m_symbolName = std::move(toMove.m_symbolName);
	m_inputData = std::move(toMove.m_inputData);
	m_cacheData = std::move(toMove.m_cacheData);
}

ska::SymbolFieldResolver& ska::SymbolFieldResolver::operator=(SymbolFieldResolver&& toMove) {
	if (this != &toMove) {
		std::unique_lock<std::mutex> lhs_lk{ m_cacheMutex, std::defer_lock };
		std::unique_lock<std::mutex> rhs_lk{ toMove.m_cacheMutex, std::defer_lock };
		std::lock(lhs_lk, rhs_lk);
		m_tableIndex = toMove.m_tableIndex;
		m_symbolName = std::move(toMove.m_symbolName);
		m_inputData = std::move(toMove.m_inputData);
		m_cacheData = std::move(toMove.m_cacheData);
	}
	return *this;
}

ska::SymbolFieldResolver::SymbolFieldResolver(const SymbolFieldResolver& toCopy) {
	std::lock_guard lock{ toCopy.m_cacheMutex };
	m_tableIndex = toCopy.m_tableIndex;
	m_symbolName = toCopy.m_symbolName;
	m_inputData = toCopy.m_inputData;
	m_cacheData = toCopy.m_cacheData;
}

ska::SymbolFieldResolver& ska::SymbolFieldResolver::operator=(const SymbolFieldResolver& toCopy) {
	if (this != &toCopy) {
		std::unique_lock<std::mutex> lhs_lk{ m_cacheMutex, std::defer_lock };
		std::unique_lock<std::mutex> rhs_lk{ toCopy.m_cacheMutex, std::defer_lock };
		std::lock(lhs_lk, rhs_lk);
		m_tableIndex = toCopy.m_tableIndex;
		m_symbolName = toCopy.m_symbolName;
		m_inputData = toCopy.m_inputData;
		m_cacheData = toCopy.m_cacheData;
	}
	return *this;
}

template<class Variant>
static ska::ScopedSymbolTable* GetTable(Variant& variant, const std::string& name, std::size_t tableIndex) {
	if (std::holds_alternative<ska::ScopedSymbolTable*>(variant)) {
		auto* containingTable = std::get<ska::ScopedSymbolTable*>(variant);
		if (containingTable != nullptr && containingTable->scopes() > tableIndex) {
			auto* tableSymbol = containingTable->child(tableIndex);
			if (tableSymbol != nullptr && tableSymbol->scopes() > 0) {
				return tableSymbol->child(tableSymbol->scopes() - 1);
			}
		} 
		SLOG_STATIC(ska::LogLevel::Warn, ska::SymbolFieldResolver) << "Unable to the enclosing table of the current symbol \"" << name << "\"";	
		return nullptr;
	}

	assert(std::holds_alternative<ska::ScriptHandleAST*>(variant));
	auto* script = std::get<ska::ScriptHandleAST*>(variant);
	assert(script != nullptr);
	SLOG_STATIC(ska::LogLevel::Warn, ska::SymbolFieldResolver) << "Found external script symbol table refered by the current symbol \"" << name << "\"";
	return &script->symbols().root();
}

ska::ScopedSymbolTable* ska::SymbolFieldResolver::lookup() {
	if (!m_closed) {
		return nullptr;
	}

	std::lock_guard lock{ m_cacheMutex };
	if (m_cacheData != nullptr) {
		return m_cacheData;
	}

	m_cacheData = GetTable(m_inputData, m_symbolName, m_tableIndex);

	return m_cacheData;
}

const ska::ScopedSymbolTable* ska::SymbolFieldResolver::lookup() const {
	if (!m_closed) {
		return nullptr;
	}

	std::lock_guard lock{ m_cacheMutex };
	if (m_cacheData != nullptr) {
		return m_cacheData;
	}

	m_cacheData = GetTable(m_inputData, m_symbolName, m_tableIndex);

	return m_cacheData;
}

bool ska::operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
	return lhs.m_tableIndex == rhs.m_tableIndex &&
		lhs.m_symbolName == rhs.m_symbolName &&
		lhs.m_inputData == rhs.m_inputData;
}

bool ska::operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
	return !operator==(lhs, rhs);
}

ska::SymbolFieldResolver::Iterator ska::SymbolFieldResolver::begin() { auto* table = lookup(); return table == nullptr ? m_defaultEmptyVector.end() : table->begin(); }
ska::SymbolFieldResolver::Iterator ska::SymbolFieldResolver::end() { auto* table = lookup(); return table == nullptr ? m_defaultEmptyVector.end() : table->end(); }
ska::SymbolFieldResolver::ConstIterator ska::SymbolFieldResolver::begin() const { auto* table = lookup(); return table == nullptr ? m_defaultEmptyVector.end() : table->begin(); }
ska::SymbolFieldResolver::ConstIterator ska::SymbolFieldResolver::end() const { auto* table = lookup(); return table == nullptr ? m_defaultEmptyVector.end() : table->end(); }

