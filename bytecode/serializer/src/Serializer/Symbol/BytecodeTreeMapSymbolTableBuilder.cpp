#include "BytecodeTreeMapSymbolTableBuilder.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Generator/Value/BytecodeScriptCache.h"

ska::bytecode::TreeMapSymbolTableBuilder::TreeMapSymbolTableBuilder(SymbolTable& table) :
	m_rootRead(&table.root()) {
}

ska::ScopedSymbolTable& ska::bytecode::TreeMapSymbolTableBuilder::walkScope(const std::vector<std::string>& parts, std::string leafName) {
	auto* currentSymbolTable = m_rootRead;

	ska::ScopedSymbolTable* parent = currentSymbolTable;
	// First part is script name native index. Last part is leaf child index.
	// avoid using both
	for (std::size_t i = 1u; i < parts.size() - 1; i++) {
		auto scopeNumericValue = std::atoi(parts[i].c_str()) ;

		if (scopeNumericValue < parent->size()) {
			// Attached to a symbol (named scope)
			// TODO check if this is still working
			/*if (scopeNumericValue >= parent->scopes()) {
				// Unexisting symbol named scope : create one
				auto* symbol = (*parent)[scopeNumericValue];
				parent = &parent->createNested(symbol);
			} else {*/
				parent = (*parent)[scopeNumericValue];
			//}
		} else {
			// Create an unnamed scope
			scopeNumericValue -= (parent->size());
			while (scopeNumericValue >= 0) {
				parent->createNested();
			}
			parent = (*parent)[scopeNumericValue];
		}
	}

	auto* symbolLeaf = (*parent)(leafName);
	// TODO check if this is still working
	return (symbolLeaf != nullptr) ? *symbolLeaf : parent->createNested(std::move(leafName));
}

ska::ScopedSymbolTable* ska::bytecode::TreeMapSymbolTableBuilder::value(const std::string& key, std::optional<std::string> name) {
	auto symbolSeekIt = m_symbolsReversedRead.find(key);

	if (symbolSeekIt == m_symbolsReversedRead.end()) {
		auto parts = StringUtils::split(key, '.');

		//parts[0] == script id => unused here because it was filtered as first
		if (parts.empty() || !name.has_value()) {
			return nullptr;
		}

		return &walkScope(std::move(parts), name.value());
	}

	return symbolSeekIt->second;
}

