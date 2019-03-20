#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Operation/Interpreter/OperationFunctionDeclaration.h"
#include "Service/Script.h"

namespace ska {

	std::vector<ska::NodeValue> InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(
		ska::Interpreter& interpreter,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {

		auto result = std::vector<ska::NodeValue>{};
		auto& functionPrototype = operateOnFunctionDeclaration.GetFunctionPrototype();
		for (auto index = 0u; index < functionPrototype.size() - 1 && operateOnFunctionCall.HasFunctionParameter(index); index++) {
			auto nodeValue = interpreter.interpret({ operateOnFunctionCall.parent, operateOnFunctionCall.GetFunctionParameterValue(index) }).asRvalue();
			result.push_back(std::move(nodeValue));
		}
		return result;
	}

	ska::NodeCell InterpreterOperationFunctionCallScript(
		ska::Interpreter& interpreter,
		ska::MemoryTable& memoryForFunctionExecution,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& node) {

		//Centers memory on the current function scope
		auto& currentExecutionMemoryZone = node.parent.memory().pointTo(memoryForFunctionExecution);

		//Creates function-memory environment scope (including creation of parameters)
		node.parent.memory().createNested();

		auto parametersValues = InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(interpreter, operateOnFunctionDeclaration, node);
		auto index = 0u;
		auto& functionPrototype = operateOnFunctionDeclaration.GetFunctionPrototype();
		for (auto& parameterValue : parametersValues) {
			auto& functionParameter = functionPrototype[index++];
			node.parent.memory().put(functionParameter.name(), std::move(parameterValue));
		}
		auto result = interpreter.interpret({ node.parent, operateOnFunctionDeclaration.GetFunctionBody() });

		//Go back to the current execution scope, while destroying the memory used during function execution
		node.parent.memory().popNested();
		node.parent.memory().pointTo(currentExecutionMemoryZone);
		return result;
	}

	ska::NodeCell InterpreterOperationFunctionCallBridge(
		ska::Interpreter& interpreter,
		BridgeFunction& bridgeCall,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {
		auto parametersValues = InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(interpreter, operateOnFunctionDeclaration, operateOnFunctionCall);
		return (bridgeCall.function)(std::move(parametersValues));
	}

}

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	auto executionContext = ExecutionContext{ node.parent, node.GetFunction() };
	auto inMemoryFunctionZone = m_interpreter.interpret(executionContext).asLvalue();
	assert(inMemoryFunctionZone.first != nullptr && inMemoryFunctionZone.second != nullptr);

	auto& functionValue = inMemoryFunctionZone.first->as<TokenVariant>();
	if (std::holds_alternative<ExecutionContext>(functionValue)) {
		auto functionExecutionContext = inMemoryFunctionZone.first->nodeval<ExecutionContext>();
		auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(functionExecutionContext);
		return InterpreterOperationFunctionCallScript(m_interpreter, *inMemoryFunctionZone.second, operateOnFunction, node);
	} else {
		auto bridgeCall = inMemoryFunctionZone.first->nodeval<std::shared_ptr<BridgeFunction>>();
		auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(executionContext);
		return InterpreterOperationFunctionCallBridge(m_interpreter, *bridgeCall, operateOnFunction, node);
	}
}
