#pragma once
#include <Utils/SubObserver.h>
#include <Utils/Observable.h>

#include "AST.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ReturnTokenEvent.h"
#include "ScopedSymbolTable.h"

namespace ska {
   
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

        const Symbol* enclosingType() const {
            return m_currentTable->enclosingType();
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
