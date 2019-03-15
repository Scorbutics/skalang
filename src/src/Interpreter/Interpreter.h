#pragma once
#include <memory>
#include <vector>
#include "InterpreterOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"

#include "Service/Binding.h"

#include "MemoryTable.h"
#include "ExecutionContext.h"

namespace ska {
    struct ReservedKeywordsPool;
	class Script;

	class Interpreter {
		using OperatorInterpreter = std::vector<std::unique_ptr<InterpreterOperatorUnit>>;
	public:
		Interpreter(const ReservedKeywordsPool& reserved);
		~Interpreter() = default;

		OperatorInterpreter build();
		NodeCell interpret(ExecutionContext node);
		NodeValue script(Script& script);

		void bind(Script& script, const std::string& functionName, BridgeFunctionPtr bridge);

		template <class ReturnType, class ... ParameterTypes>
		void bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
//			auto bridge = m_binding.bindFunction<ReturnType, ParameterTypes...>(functionName, std::move(f));
//			m_memory.emplace(functionName, NodeValue{ std::move(bridge) });
		}

	private:
		//Binding m_binding;
		//SymbolTable& m_symbols;
		//MemoryTable m_memory;
		OperatorInterpreter m_operatorInterpreter;
	};
}
