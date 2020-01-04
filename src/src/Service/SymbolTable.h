#pragma once
#include "Container/sorted_observable.h"

#include "NodeValue/AST.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Event/ImportTokenEvent.h"

#include "ScopedSymbolTable.h"

namespace ska {
   
	class StatementParser;

	class SymbolTable;
	class ParserListenerLock {
	public:
		ParserListenerLock(SymbolTable& symbolTable, StatementParser& parser);
		ParserListenerLock(ParserListenerLock&&) noexcept;
		ParserListenerLock(const ParserListenerLock&) = delete;
		ParserListenerLock& operator=(ParserListenerLock&&) = delete;
		ParserListenerLock& operator=(const ParserListenerLock&) = delete;
		
		~ParserListenerLock();
		void release();
	private:
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	class SymbolTable :
    	public PriorityObserver<VarTokenEvent>,
   		public PriorityObserver<BlockTokenEvent>,
    	public PriorityObserver<FunctionTokenEvent>,
    	public PriorityObserver<ReturnTokenEvent>, 
		public PriorityObserver<ImportTokenEvent>,
		public PriorityObserver<ScriptLinkTokenEvent> {

    	using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class ParserListenerLock;
	public:
		SymbolTable();
		SymbolTable(StatementParser& parser);
		virtual ~SymbolTable();
		
		[[nodiscard]]
		ParserListenerLock listenParser(StatementParser& parser);

		auto* operator[](const std::string& key) { return (*m_currentTable)[key]; }
    	const auto* operator[](const std::string& key) const { return (*m_currentTable)[key]; }

		ScopedSymbolTable::ChildrenScopedSymbolTable& nested() { return m_currentTable->children(); }
		const ScopedSymbolTable::ChildrenScopedSymbolTable& nested() const { return m_currentTable->children(); }

		ScopedSymbolTable* current() { return m_currentTable; }
		const ScopedSymbolTable* current() const { return m_currentTable; }

		const Symbol* enclosingType() const { 
			return m_currentTable->owner();
		}

	private:
		bool match(const VarTokenEvent&);
		bool nestedTable(const BlockTokenEvent&);
		bool matchFunction(const FunctionTokenEvent&);
		bool matchReturn(const ReturnTokenEvent&);
		bool matchImport(const ImportTokenEvent&);
		bool matchScriptLink(const ScriptLinkTokenEvent&);

		void internalListenParser(StatementParser& parser);
		void internalUnlistenParser();

		StatementParser* m_parser = nullptr;
    	std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
	};
}
