#include "BytecodeTreeMapSymbolTableBuilder.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"
#include "Base/Values/Strings/StringUtils.h"

ska::bytecode::TreeMapSymbolTableBuilder::TreeMapSymbolTableBuilder(SymbolTable& table) :
	m_rootRead(&table.root()) {
}

ska::ScopedSymbolTable& ska::bytecode::TreeMapSymbolTableBuilder::walkScope(const std::vector<std::string>& parts) {
	auto* currentSymbolTable = m_rootRead;

	ska::ScopedSymbolTable* parent = currentSymbolTable;
	for (std::size_t i = 1u; i < parts.size(); i++) {
		const auto scopeNumericValue = std::atoi(parts[i].c_str());

		while (scopeNumericValue >= parent->scopes()) {
			parent->createNested();
		}
		parent = currentSymbolTable->child(scopeNumericValue);
	}

	return *parent;
}

ska::Symbol* ska::bytecode::TreeMapSymbolTableBuilder::value(const std::string& key, std::optional<std::string> name) {
	auto symbolSeekIt = m_symbolsReversedRead.find(key);

	if (symbolSeekIt == m_symbolsReversedRead.end()) {
		auto parts = StringUtils::split(key, '.');
		
		//parts[0] == script id => unused here because it was filtered as first
		if (parts.empty() || !name.has_value()) {
			return nullptr;
		}
		
		auto& parentScope = walkScope(std::move(parts));
		auto& parentSymbol = parentScope.emplace(name.value());
		//parentScope.createNested(&parentSymbol);
		return &parentSymbol;
	}

	return symbolSeekIt->second;
}
