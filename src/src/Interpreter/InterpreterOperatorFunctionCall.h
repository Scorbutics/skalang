#pragma once

#include "InterpreterOperator.h"
#include "Operation/Interpreter/OperationFunctionCall.h"
#include "Operation/Interpreter/OperationFunctionDeclaration.h"
#include "Interpreter/MemoryTablePtr.h"

namespace ska {
	class Script;
	class Interpreter;

	NodeCell InterpreterOperationFunctionCallScriptWithParams(
		Script& parentScript,
		Interpreter& interpreter,
		MemoryTablePtr& memoryForFunctionExecution,
		Operation<Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		std::vector<NodeValue> parametersValues);

	SKALANG_INTERPRETER_OPERATOR_DEFINE(Operator::FUNCTION_CALL);
}
