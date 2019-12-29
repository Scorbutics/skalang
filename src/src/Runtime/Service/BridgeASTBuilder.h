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
#include "Event/ReturnTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Container/sorted_observable.h"

#include "Service/Matcher/MatcherType.h"
#include "Runtime/Value/BridgeImport.h"
#include "BridgeFunction.h"

namespace ska {
	class SymbolTable;
	class TypeBuilder;
	class SymbolTableUpdater;
	struct ReservedKeywordsPool;
	class StatementParser;

	class BridgeASTBuilder;
	class BridgeASTBuilderSymbolTableLock {
	public:
		BridgeASTBuilderSymbolTableLock(BridgeASTBuilder& factory, SymbolTable& table);
		BridgeASTBuilderSymbolTableLock(BridgeASTBuilderSymbolTableLock&&) noexcept;
		BridgeASTBuilderSymbolTableLock(const BridgeASTBuilderSymbolTableLock&) = delete;
		BridgeASTBuilderSymbolTableLock& operator=(BridgeASTBuilderSymbolTableLock&&) = delete;
		BridgeASTBuilderSymbolTableLock& operator=(const BridgeASTBuilderSymbolTableLock&) = delete;

		~BridgeASTBuilderSymbolTableLock();
		void release();
	private:
		BridgeASTBuilder& m_factory;
		SymbolTable& m_symbolTable;
		bool m_freed = false;
	};

	//http://coliru.stacked-crooked.com/a/8efdf80ac4082e22
	class BridgeASTBuilder :
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent>,
		public observable_priority_queue<ImportTokenEvent>,
		public observable_priority_queue<ReturnTokenEvent> {
		friend class BridgeASTBuilderSymbolTableLock;
	public:
		BridgeASTBuilder(TypeBuilder& typeBuilder, SymbolTableUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved);
		virtual ~BridgeASTBuilder();

		BridgeImportRaw import(StatementParser& parser, ScriptAST& script, std::pair<std::string, std::string> import);

		template<class ... Types>
		std::vector<std::string> buildTypes() {
			auto ss = std::vector<std::string>{};
			int _[] = {0, (buildType<Types>(ss), 0)...};
			(void)_;
			return ss;
		}


		ASTNodePtr makeFunction(ScriptAST& script, BridgeFunction data);

		//ASTNodePtr makeFunction(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames, BridgeConstructorData = {});
		//ASTNodePtr makeFunction(ScriptAST& script, const Type& fullTypeFunction, BridgeConstructorData = {});

	private:
		ASTNodePtr makeFunctionPrototype(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames);
		ASTNodePtr makeFunctionPrototype(ScriptAST& script, const Type& fullTypeFunction);
		ASTNodePtr makeFunctionDeclaration(ScriptAST& script, ASTNodePtr prototype, std::vector<BridgeField> fieldList);
		ASTNodePtr makeFunctionParameterOrReturnType(ScriptAST& script, ASTNodePtr nodeType, std::size_t parameterIndex, std::size_t totalParameters);
		ASTNodePtr makeFunctionName(ScriptAST& script, const std::string& name);
		ASTNodePtr makeFunctionPrototype(ScriptAST& script, ASTNodePtr nameNode, std::vector<ASTNodePtr> parametersAndReturn);
		ASTNodePtr makeVariable(ScriptAST& script, const std::string& name, ASTNodePtr value);

		ASTNodePtr makeCustomObjectReturn(ScriptAST& script, std::vector<BridgeField> fieldList);
		std::vector<ASTNodePtr> makeFunctionInputOutput(ScriptAST& script, const std::vector<std::string>& typeNames);
		std::vector<ASTNodePtr> makeFunctionInputOutput(ScriptAST& script, const Type& fullTypeFunction);

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

		std::unordered_map<const SymbolTable*, int> m_symbolTableLockCounter;
		TypeBuilder& m_typeBuilder;
		SymbolTableUpdater& m_symbolTypeUpdater;
		const ReservedKeywordsPool& m_reserved;
		MatcherType m_matcherType;
	};
}
