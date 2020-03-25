#pragma once
#include <unordered_map>
#include <memory>
#include "NodeValue/Symbol.h"
#include "Container/order_indexed_string_map.h"
#include "SymbolFactory.h"

namespace ska {
	class SymbolTable;
	class ScriptAST;

	class ScopedSymbolTable : 
		public SymbolFactory {
	using ChildrenScopedSymbolTable = std::vector<std::unique_ptr<ScopedSymbolTable>>;

	public:
		ScopedSymbolTable(ScopedSymbolTable& parent) :
			m_parent(parent) {
		}

		ScopedSymbolTable() = default;
		~ScopedSymbolTable() = default;

		ScopedSymbolTable& parent();
		const ScopedSymbolTable& parent() const;

		ScopedSymbolTable& createNested(Symbol* s = nullptr, bool isExported = false);
		Symbol& emplace(std::string name);
		Symbol& emplace(std::string name, const ScriptAST& script);

		const Symbol* owner() const {
			return m_parentSymbol == nullptr && &m_parent != this ? m_parent.owner() : m_parentSymbol;
		}

		const Symbol* directOwner() const { return m_parentSymbol; }

		Symbol* directOwner() { return m_parentSymbol; }

		bool exported() const { return m_exported; }

		bool changeTypeIfRequired(const std::string& symbol, const Type& value);

		const Symbol* operator[](const std::string& key) const;
		Symbol* operator[](const std::string& key);

		const Symbol* operator()(const std::string& key) const;
		Symbol* operator()(const std::string& key);

		const Symbol* operator[](std::size_t index) const;
		Symbol* operator[](std::size_t index);

		auto end() const { return m_symbols.end(); }
		auto begin() const { return m_symbols.begin(); }
		auto end() { return m_symbols.end(); }
		auto begin() { return m_symbols.begin(); }

		ScopedSymbolTable* child(std::size_t index) { return index < m_children.size() ? m_children[index].get() : nullptr; }
		const ScopedSymbolTable* child(std::size_t index) const { return index < m_children.size() ? m_children[index].get() : nullptr; }

		const Symbol* back() const { return m_symbols.empty() ? nullptr : &m_symbols.back(); }
		Symbol* back() { return m_symbols.empty() ? nullptr : &m_symbols.back(); }
		std::size_t size() const { return m_symbols.size(); }
		bool empty() const { return m_symbols.empty(); }
		std::size_t scopes() const { return m_children.size(); }
	private:
		Symbol& emplace(Symbol symbol);

		order_indexed_string_map<Symbol> m_symbols;
		ChildrenScopedSymbolTable m_children;
		ScopedSymbolTable& m_parent = *this;
		Symbol* m_parentSymbol = nullptr;
		bool m_exported = false;
	};
}
