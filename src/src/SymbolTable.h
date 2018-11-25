#pragma once
#include <Utils/SubObserver.h>
#include <Utils/Observable.h>

#include "AST.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ReturnTokenEvent.h"
#include "ExpressionType.h"
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
		Symbol& emplace(std::string name, Type type = Type{});
		
        void link(Symbol& s) {
            m_parentSymbol = &s;
        }

        Symbol* parentSymbol() {
            return m_parentSymbol;
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

    class Parser;

	class SymbolTable :
        public SubObserver<VarTokenEvent>,
       	public SubObserver<BlockTokenEvent>,
        public SubObserver<FunctionTokenEvent>,
        public SubObserver<ReturnTokenEvent> {

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    public:
		SymbolTable(Parser& parser);
		~SymbolTable() = default;
		
		auto* operator[](const std::string& key) {
            return (*m_currentTable)[key];
        }

        const auto* operator[](const std::string& key) const {
            return (*m_currentTable)[key];
        }

		ScopedSymbolTable::ChildrenScopedSymbolTable& nested() {
			return m_currentTable->children();
		}

		const ScopedSymbolTable::ChildrenScopedSymbolTable& nested() const {
			return m_currentTable->children();
		}

		ScopedSymbolTable* current() {
			return m_currentTable;
		}

		const ScopedSymbolTable* current() const {
			return m_currentTable;
		}

    private:
		bool match(const VarTokenEvent&);
		bool nestedTable(const BlockTokenEvent&);
		bool matchFunction(const FunctionTokenEvent&);
		bool matchReturn(const ReturnTokenEvent&);

        std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
    };
}
