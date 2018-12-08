#pragma once
#include <unordered_map>
#include <memory>
#include "Symbol.h"

namespace ska {
	class SymbolTable;

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
		ScopedSymbolTable& createNested();
		Symbol& emplace(std::string name);
		
        void link(Symbol& s) {
            m_parentSymbol = &s;
        }

        Symbol* parentSymbol() {
            return m_parentSymbol;
        }

        const Symbol* enclosingType() const {
            return m_parent.m_parentSymbol;
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

	private:
        std::unordered_map<std::string, Symbol> m_symbols;
		ChildrenScopedSymbolTable m_children;
		ScopedSymbolTable& m_parent = *this;
        Symbol* m_parentSymbol = nullptr;
	};
}
