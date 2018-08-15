#pragma once
#include <Utils/SubObserver.h>
#include <Utils/Observable.h>

#include "AST.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"

namespace ska {
    	struct Symbol {
        	Operator category;
    	};

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
		void emplace(std::string name, Operator op);
		
		Symbol* operator[](const std::string& key) {
                	auto valueIt = m_symbols.find(key);
			if(valueIt == m_symbols.end()) {
				return &m_parent == this ? nullptr : m_parent[key];
			}
			return &m_symbols.at(key);
                }

                const Symbol* operator[](const std::string& key) const {
                    const auto valueIt = m_symbols.find(key);
		    if(valueIt == m_symbols.end()) {
			    return &m_parent == this ? nullptr : m_parent[key];
		    }
		    return &m_symbols.at(key);
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
	};

	class SymbolTable :
        public SubObserver<VarTokenEvent>,
       	public SubObserver<BlockTokenEvent> {

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    	public:
		SymbolTable(Observable<VarTokenEvent>& variableDeclarer, Observable<BlockTokenEvent>& scopeMaker);
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

    	private:
        	bool match(VarTokenEvent&);
		bool nestedTable(BlockTokenEvent&);

        	std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
	};
}
