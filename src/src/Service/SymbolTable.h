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
#include "SymbolTableOperation.h"

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
		public PriorityObserver<ScriptLinkTokenEvent>,
		public SymbolFactory {

    	using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class ParserListenerLock;
	public:
		SymbolTable();
		SymbolTable(StatementParser& parser);
		virtual ~SymbolTable();
		
		[[nodiscard]]
		ParserListenerLock listenParser(StatementParser& parser);

		Symbol* operator[](std::size_t index) { return (*m_currentTable)[index]; }
		const Symbol* operator[](std::size_t index) const { return (*m_currentTable)[index];  }

		Symbol* operator[](const std::string& key) { return (*m_currentTable)[key]; }
		const Symbol* operator[](const std::string& key) const { return (*m_currentTable)[key]; }

		Symbol* operator()(const std::string& key) { return (*m_currentTable)(key); }
		const Symbol* operator()(const std::string& key) const { return (*m_currentTable)(key); }

		const Symbol* enclosingType() const {
			return m_currentTable->owner();
		}

		std::size_t size() const {
			return m_currentTable->size();
		}

		std::size_t scopes() const {
			return m_currentTable->scopes();
		}

		bool changeTypeIfRequired(const std::string& symbolName, const Type& value);
		const Symbol* lookup(SymbolTableLookup strategy, SymbolTableNested depth = SymbolTableNested::current()) const;
		Symbol* lookup(SymbolTableLookup strategy, SymbolTableNested depth = SymbolTableNested::current());

		auto begin() { return m_rootTable->begin(); }
		auto end() { return m_rootTable->end(); }
		auto begin() const { return m_rootTable->begin(); }
		auto end() const  { return m_rootTable->end(); }

		ScopedSymbolTable& root() { return *m_rootTable; }
		const ScopedSymbolTable& root() const { return *m_rootTable; }

	private:
		bool match(VarTokenEvent&);
		bool nestedTable(const BlockTokenEvent&);
		bool matchFunction(FunctionTokenEvent&);
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
