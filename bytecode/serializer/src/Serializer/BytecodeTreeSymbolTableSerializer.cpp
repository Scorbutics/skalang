#include "BytecodeTreeSymbolTableSerializer.h"
#include "Service/SymbolTable.h"

void ska::bytecode::TreeSymbolTableSerializer::serialize(const SymbolTable& table) {
	serialize(table.root(), "");
}

void ska::bytecode::TreeSymbolTableSerializer::serialize(const ScopedSymbolTable& table, const std::string& depth) {
	auto childIndex = std::size_t{ 0 };	
	for (const auto& symbol : table) {
		serialize(*symbol, depth, childIndex++);
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
			serialize(*childTable, nextDepth);
		} else {
			// Or a symbol related symbol table, therefore here we query the depth of the already existing symbol
			auto seekOwnerIt = m_symbols.find(owner);
			if (seekOwnerIt == m_symbols.end()) {
				throw std::runtime_error("invalid symbol table : a child scoped symbol table has an owner in its script that is not currently known.");
			}
			serialize(*childTable, seekOwnerIt->second);
		}
		
	}
}

void ska::bytecode::TreeSymbolTableSerializer::serialize(const Symbol& symbol, const std::string& depth, std::size_t childIndex) {
	const auto existingSymbolIt = m_symbols.find(&symbol);
	if (existingSymbolIt == m_symbols.end()) {
		m_symbols.emplace(&symbol, buildKey(depth, childIndex));
	}
}

std::string ska::bytecode::TreeSymbolTableSerializer::buildKey(const std::string& depth, std::size_t childIndex) {
	auto strChildIndex = std::to_string(childIndex);
	return depth.empty() ? strChildIndex : depth + "." + strChildIndex;
}
