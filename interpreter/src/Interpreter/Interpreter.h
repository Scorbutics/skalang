#pragma once
#include <memory>
#include <vector>
#include "InterpreterOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"

#include "Interpreter/Value/ExecutionContext.h"

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class TypeCrosser;

	class Interpreter {
		using OperatorInterpreter = std::vector<std::unique_ptr<InterpreterOperatorUnit>>;
	public:
		Interpreter(const ReservedKeywordsPool& reserved, const TypeCrosser& typeCrosser);
		~Interpreter() = default;

		OperatorInterpreter build();
		NodeCell interpret(ExecutionContext node);
		NodeValue script(Script& script);

	private:
		OperatorInterpreter m_operatorInterpreter;
		const TypeCrosser& m_typeCrosser;
	};
}
