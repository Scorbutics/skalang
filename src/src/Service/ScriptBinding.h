#pragma once
#include <functional>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <Data/Events/EventDispatcher.h>

#include "Container/sorted_observable.h"
#include "Interpreter/BridgeFunction.h"
#include "Service/BindingFactory.h"
#include "Event/VarTokenEvent.h"
#include "Service/Script.h"
#include "ScriptCache.h"

namespace ska {
    class SymbolTable;
	
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

	
	private:
		TypeBuilder& m_typeBuilder;
		SymbolTableTypeUpdater& m_symbolTypeUpdater;
		BindingFactory m_functionBinder;
		std::vector<BridgeFunctionPtr> m_bindings;
		std::string m_name;
		Script m_script;
		ASTNodePtr m_scriptMemoryNode;
	};
}
