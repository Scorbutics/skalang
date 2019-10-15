#pragma once
#include <functional>
#include <cassert>
#include <unordered_map>

#include "NodeValue/ASTNodePtr.h"
#include "Runtime/Value/StringShared.h"
#include "Base/Patterns/Observable.h"
#include "Base/Meta/TupleUtils.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Container/sorted_observable.h"

#include "Service/Matcher/MatcherType.h"
#include "Runtime/Value/BridgeImport.h"

namespace ska {
	class SymbolTable;
	class TypeBuilder;
	class SymbolTableUpdater;
	struct ReservedKeywordsPool;
	class StatementParser;

	class BindingFactory;
	class BindingFactorySymbolTableLock {
	public:
		BindingFactorySymbolTableLock(BindingFactory& factory, SymbolTable& table);
		BindingFactorySymbolTableLock(BindingFactorySymbolTableLock&&) noexcept;
		BindingFactorySymbolTableLock(const BindingFactorySymbolTableLock&) = delete;
		BindingFactorySymbolTableLock& operator=(BindingFactorySymbolTableLock&&) = delete;
		BindingFactorySymbolTableLock& operator=(const BindingFactorySymbolTableLock&) = delete;

		~BindingFactorySymbolTableLock();
		void release();
	private:
		BindingFactory& m_factory;
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class BindingFactory :
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent>,
		public observable_priority_queue<ImportTokenEvent> {
		friend class BindingFactorySymbolTableLock;
	public:
		BindingFactory(TypeBuilder& typeBuilder, SymbolTableUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved);
		virtual ~BindingFactory();

		BridgeImportRaw import(StatementParser& parser, ScriptAST& script, std::pair<std::string, std::string> import);

		template<class ... Types>
		std::vector<std::string> buildTypes() {
			auto ss = std::vector<std::string>{};
			int _[] = {0, (buildType<Types>(ss), 0)...};
			(void)_;
			return ss;
		}

		ASTNodePtr bindSymbol(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames);

	private:
		void internalUnlisten(SymbolTable& symbolTable);
		void internalListen(SymbolTable& symbolTable);

		template <class T>
		void buildType(std::vector<std::string>& ss) {
			if constexpr (std::is_same<T, StringShared>()) {
				ss.push_back("string");
			} else if constexpr (std::is_same<T, int>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, long>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, std::size_t>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, float>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, bool>()) {
				ss.push_back("bool");
			} else if constexpr (std::is_same<T, double>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, void>()) {
				ss.push_back("void");
			} else {
				throw std::runtime_error("Invalid type for bridge function");
			}
		}

		ASTNodePtr createImport(StatementParser& parser, ScriptAST& script, Token scriptPathToken);

		TypeBuilder& m_typeBuilder;
		SymbolTableUpdater& m_symbolTypeUpdater;
		const ReservedKeywordsPool& m_reserved;
		MatcherType m_matcherType;
	};
}
