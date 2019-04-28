#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <Data/Events/EventDispatcher.h>

#include "Container/sorted_observable.h"
#include "Interpreter/Value/BridgeFunction.h"
#include "Service/BindingFactory.h"
#include "Event/VarTokenEvent.h"
#include "Interpreter/Value/Script.h"
#include "ScriptCache.h"

namespace ska {
    class SymbolTable;
	class StatementParser;

	class ScriptBridge :
        protected observable_priority_queue<VarTokenEvent> {
	public:
		ScriptBridge(
			ScriptCache& cache,
			std::string scriptName,
			TypeBuilder& typeBuilder,
			SymbolTableTypeUpdater& symbolTypeUpdater,
			const ReservedKeywordsPool& reserved);
			
		virtual ~ScriptBridge();

		void build();

		template <class ReturnType, class ... ParameterTypes>
		void bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
			assert(m_scriptMemoryNode == nullptr);
			m_bindings.push_back(m_functionBinder.bindFunction<ReturnType, ParameterTypes...>(m_script, functionName, std::move(f)));
		}

		void bindGenericFunction(const std::string& functionName, std::vector<std::string> typeNames, decltype(BridgeFunction::function) f) {
			assert(m_scriptMemoryNode == nullptr);
			m_bindings.push_back(m_functionBinder.bindGenericFunction(m_script, functionName, std::move(typeNames), std::move(f)));
		}

		void import(StatementParser& parser, Interpreter& interpreter, std::vector<std::pair<std::string, std::string>> imports) {			
			m_imports.emplace_back(m_functionBinder.import(parser, m_script, interpreter, std::move(imports)));
		}
	
		MemoryTablePtr createMemory() { return m_script.createMemory(); }

		NodeValue callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues);
		MemoryLValue accessMemory(std::string importName, std::string field);

	private:
		TypeBuilder& m_typeBuilder;
		SymbolTableTypeUpdater& m_symbolTypeUpdater;
		BindingFactory m_functionBinder;
		std::string m_name;
		Script m_script;
		ASTNodePtr m_scriptMemoryNode;
		ScriptCache& m_cache;

		std::vector<ASTNodePtr> m_imports;
		std::vector<BridgeMemory> m_bindings;
	};
}
