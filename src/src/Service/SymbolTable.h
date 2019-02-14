#pragma once
#include <Utils/Observer.h>

#include "NodeValue/AST.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/BridgeTokenEvent.h"
#include "Event/ImportTokenEvent.h"

#include "ScopedSymbolTable.h"

namespace ska {
   
    class StatementParser;

	class SymbolTable :
        public Observer<VarTokenEvent>,
       	public Observer<BlockTokenEvent>,
        public Observer<FunctionTokenEvent>,
        public Observer<ReturnTokenEvent>, 
		public Observer<ImportTokenEvent>,
		public Observer<BridgeTokenEvent> {

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    public:
		SymbolTable();
		SymbolTable(StatementParser& parser);
		virtual ~SymbolTable();
		
		void listenParser(StatementParser& parser);
		void unlistenParser();

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
            return m_currentTable->owner();
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
		bool matchImport(const ImportTokenEvent&);
		bool matchBridge(const BridgeTokenEvent&);

		bool erase(const std::string& name) {
			return m_currentTable->erase(name);
		}

		StatementParser* m_parser = nullptr;
        std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
    };
}
