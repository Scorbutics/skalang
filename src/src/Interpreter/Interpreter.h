#pragma once
#include <memory>
#include <vector>
#include "InterpreterOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "MemoryTable.h"
#include "Service/Binding.h"

namespace ska {
    struct ReservedKeywordsPool;
	class Script;

	class Interpreter {
		using OperatorInterpreter = std::vector<std::unique_ptr<InterpreterOperatorUnit>>;
	public:
		Interpreter(SymbolTable& symbols, const ReservedKeywordsPool& reserved);
		~Interpreter() = default;

		OperatorInterpreter build();
		NodeCell interpret(ASTNode& node);
		NodeValue script(Script& script);

		void bind(const std::string& functionName, BridgeFunctionPtr bridge) {
			m_memory.emplace(functionName, NodeValue{ std::move(bridge) });
		}

		template <class ReturnType, class ... ParameterTypes>
		void bindFunction(const std::string& functionName, std::function<ReturnType(ParameterTypes...)> f) {
//			auto bridge = m_binding.bindFunction<ReturnType, ParameterTypes...>(functionName, std::move(f));
//			m_memory.emplace(functionName, NodeValue{ std::move(bridge) });
		}

	private:
		//Binding m_binding;
		SymbolTable& m_symbols;
		MemoryTable m_memory;
		OperatorInterpreter m_operatorInterpreter;
	};
}
