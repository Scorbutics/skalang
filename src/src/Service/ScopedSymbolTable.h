#pragma once
#include <unordered_map>
#include <memory>
#include "NodeValue/Symbol.h"

namespace ska {
	class SymbolTable;
	class Script;
	
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

		ScopedSymbolTable& createNested();
		Symbol& emplace(std::string name);
		Symbol& emplace(std::string name, Script& script);

        void link(Symbol& s) {
            m_parentSymbol = &s;
        }

        const Symbol* owner() const {
            return m_parentSymbol == nullptr && &m_parent != this ? m_parent.owner() : m_parentSymbol;
        }

		Symbol* operator[](const std::string& key) {
            auto valueIt = m_symbols.find(key);
			if(valueIt == m_symbols.end()) {
				return &m_parent == this ? nullptr : m_parent[key];
			}
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
        }

        const Symbol* operator[](const std::string& key) const {
            const auto valueIt = m_symbols.find(key);
            if(valueIt == m_symbols.end()) {
                return &m_parent == this ? nullptr : m_parent[key];
            }
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
        }
		
		Symbol* operator()(const std::string& key) {
			auto valueIt = m_symbols.find(key);
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
		}

		const Symbol* operator()(const std::string& key) const {
			const auto valueIt = m_symbols.find(key);
			return valueIt == m_symbols.end() ? nullptr : &(valueIt->second);
		}

		ChildrenScopedSymbolTable& children() {
			return m_children;
		}
		
		const ChildrenScopedSymbolTable& children() const {
			return m_children;
		}

		bool erase(const std::string& name) {
			auto valueIt = m_symbols.find(name);
			if (valueIt == m_symbols.end()) {				
				return (&m_parent != this) ? m_parent.erase(name) : false;
			}
			m_symbols.erase(name);
			return true;
		}

		auto begin() {
			return m_symbols.begin();
		}

		auto end() {
			return m_symbols.end();
		}

	private:
		Symbol& emplace(Symbol symbol);

        std::unordered_map<std::string, Symbol> m_symbols;
		ChildrenScopedSymbolTable m_children;
		ScopedSymbolTable& m_parent = *this;
        Symbol* m_parentSymbol = nullptr;
	};
}
