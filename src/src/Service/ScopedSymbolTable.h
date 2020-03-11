#pragma once
#include <unordered_map>
#include <memory>
#include "NodeValue/Symbol.h"

namespace ska {
	class SymbolTable;
	class ScriptAST;

	class ScopedSymbolTable {
	using ChildrenScopedSymbolTable = std::vector<std::unique_ptr<ScopedSymbolTable>>;

	friend class SymbolTable;
	public:
		ScopedSymbolTable(ScopedSymbolTable& parent) :
			m_parent(parent) {
		}

		ScopedSymbolTable() = default;
		~ScopedSymbolTable() = default;

		ScopedSymbolTable& parent();
		const ScopedSymbolTable& parent() const;

		ScopedSymbolTable& createNested(Symbol* s = nullptr);
		Symbol& emplace(std::string name);
		Symbol& emplace(std::string name, const ScriptAST& script);

		const Symbol* owner() const {
			return m_parentSymbol == nullptr && &m_parent != this ? m_parent.owner() : m_parentSymbol;
		}

		const Symbol* directOwner() const {
			return m_parentSymbol;
		}

		bool changeTypeIfRequired(const std::string& symbol, Type value);

		const Symbol* operator[](const std::string& key) const {
			const auto valueIt = m_symbols.find(key);
			if(valueIt == m_symbols.end()) {
					return &m_parent == this ? nullptr : m_parent[key];
			}
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
		}

		const Symbol* operator()(const std::string& key) const {
			const auto valueIt = m_symbols.find(key);
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
		}

		const ChildrenScopedSymbolTable& children() const {
			return m_children;
		}

	private:
		Symbol* operator[](const std::string& key) {
			auto valueIt = m_symbols.find(key);
			if (valueIt == m_symbols.end()) {
				return &m_parent == this ? nullptr : m_parent[key];
			}
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
		}
		Symbol& emplace(Symbol symbol);

		std::unordered_map<std::string, Symbol> m_symbols;
		ChildrenScopedSymbolTable m_children;
		ScopedSymbolTable& m_parent = *this;
		Symbol* m_parentSymbol = nullptr;
	};
}
