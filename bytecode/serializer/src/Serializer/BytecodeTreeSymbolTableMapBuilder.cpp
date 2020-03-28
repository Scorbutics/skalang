#include "BytecodeTreeSymbolTableMapBuilder.h"
#include "Runtime/Value/SerializerOutput.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"

ska::bytecode::TreeSymbolTableMapBuilder::TreeSymbolTableMapBuilder(const SymbolTable& table) {
	store(table, "");
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const SymbolTable& table, const std::string& defaultDepth) {
	store(table.root(), defaultDepth);
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const ScopedSymbolTable& table, const std::string& depth) {
	auto childIndex = std::size_t{ 0 };	
	for (const auto& symbol : table) {
		store(*symbol, depth, childIndex++);
	}

	const auto childrenScopes = table.scopes();
	for (std::size_t index = 0; index < childrenScopes; index++) {
		const auto* childTable = table.child(index);
		if (childTable == nullptr) {
			throw std::runtime_error("invalid symbol table \"" + std::to_string(index) + "\"");
		}

		const auto* owner = childTable->directOwner();
		if (owner == nullptr) {
			// Either a non owned symbol table => we enqueue it after all children indexes
			auto nextDepth = buildKey(depth, index + childIndex);
			store(*childTable, nextDepth);
		} else {
			// Or a symbol related symbol table, therefore here we query the depth of the already existing symbol
			auto seekOwnerIt = m_symbols.find(owner);
			if (seekOwnerIt == m_symbols.end()) {
				throw std::runtime_error("invalid symbol table : a child scoped symbol table has an owner in its script that is not currently known.");
			}
			store(*childTable, seekOwnerIt->second);
		}
		
	}
}

void ska::bytecode::TreeSymbolTableMapBuilder::store(const Symbol& symbol, const std::string& depth, std::size_t childIndex) {
	const auto existingSymbolIt = m_symbols.find(&symbol);
	if (existingSymbolIt == m_symbols.end()) {
		auto key = buildKey(depth, childIndex);
		m_symbols.emplace(&symbol, key);
		m_symbolsReversed.emplace(std::move(key), &symbol);
	}
}

std::string ska::bytecode::TreeSymbolTableMapBuilder::buildKey(const std::string& depth, std::size_t childIndex) {
	auto strChildIndex = std::to_string(childIndex);
	return depth.empty() ? strChildIndex : depth + "." + strChildIndex;
}

std::string ska::bytecode::TreeSymbolTableMapBuilder::key(const Symbol& symbol) const {
	auto symbolSeekIt = m_symbols.find(&symbol);
	return symbolSeekIt == m_symbols.end() ? "" : symbolSeekIt->second;
}

void ska::bytecode::TreeSymbolTableMapBuilder::write(SerializerOutput& output, SymbolTableSerializer& serializer) const {
	serializer.writeFull(output, m_symbolsReversed);
}
