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

	class ScopedSymbolTable {
	public:
		ScopedSymbolTable(ScopedSymbolTable& parent) :
			m_parent(parent) {
		}

		ScopedSymbolTable() = default;
		~ScopedSymbolTable() = default;

		ScopedSymbolTable& parent();
		ScopedSymbolTable& createNested();
		void emplace(std::string name, Operator op);
	private:
        	std::unordered_map<std::string, Symbol> m_symbols;
		std::vector<std::unique_ptr<ScopedSymbolTable>> m_children;
		ScopedSymbolTable& m_parent = *this;
	};

	class SymbolTable :
        public SubObserver<VarTokenEvent>,
       	public SubObserver<BlockTokenEvent>	{

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    	public:
		SymbolTable(Observable<VarTokenEvent>& variableDeclarer, Observable<BlockTokenEvent>& scopeMaker);
		~SymbolTable() = default;

    	private:
        	bool match(VarTokenEvent&);
		bool nestedTable(BlockTokenEvent&);

        	std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
	};
}
