#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>);

namespace ska {

	std::vector<ska::NodeValue> InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(
		ska::Interpreter& interpreter,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {

		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Computing parameters value";

		auto result = std::vector<ska::NodeValue>{};
		auto& functionPrototype = operateOnFunctionDeclaration.GetFunctionPrototype();
		for (auto index = 0u; index < functionPrototype.size() - 1 && operateOnFunctionCall.HasFunctionParameter(index); index++) {
			SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Building parameter " << index << "...";
			auto nodeValue = interpreter.interpret({ operateOnFunctionCall.parent, operateOnFunctionCall.GetFunctionParameterValue(index) }).asRvalue();
			result.push_back(std::move(nodeValue.object));
		}
		return result;
	}

	ska::NodeCell InterpreterOperationFunctionCallScriptWithParams(
		ska::Script& parentScript,
		ska::Interpreter& interpreter,
		ska::MemoryTablePtr& memoryForFunctionExecution,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		std::vector<ska::NodeValue> parametersValues) {
		auto index = 0u;
		auto& functionPrototype = operateOnFunctionDeclaration.GetFunctionPrototype();

		//Centers memory on the current function scope
		auto currentExecutionMemoryZone = parentScript.pointMemoryTo(memoryForFunctionExecution);

		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Creating nested memory context to execute function";

		//Creates function-memory environment scope (including creation of parameters)
		auto lock = parentScript.pushNestedMemory(true);

		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Setting parameters values";

		for (auto& parameterValue : parametersValues) {
			auto& functionParameter = functionPrototype[index++];
			parentScript.putMemory(functionParameter.name(), std::move(parameterValue));
		}
		auto result = interpreter.interpret({ parentScript, operateOnFunctionDeclaration.GetFunctionBody() });

		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Poping nested memory and going back to call context";

		//Go back to the current execution scope, while destroying the memory used during function execution
		lock.release();
		parentScript.pointMemoryTo(currentExecutionMemoryZone);
		return result;
	}

	ska::NodeCell InterpreterOperationFunctionCallScript(
		ska::Interpreter& interpreter,
		ska::MemoryTablePtr& memoryForFunctionExecution,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& node) {
		auto parametersValues = InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(interpreter, operateOnFunctionDeclaration, node);
		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Function is a script, transfering parameters";
		return InterpreterOperationFunctionCallScriptWithParams(node.parent, interpreter, memoryForFunctionExecution, operateOnFunctionDeclaration, std::move(parametersValues));
	}

	ska::NodeCell InterpreterOperationFunctionCallBridge(
		ska::Interpreter& interpreter,
		BridgeFunction& bridgeCall,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {
		auto parametersValues = InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(interpreter, operateOnFunctionDeclaration, operateOnFunctionCall);
		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Function is a bridge, transfering parameters";
		return NodeRValue{(bridgeCall.function)(operateOnFunctionCall.parent, std::move(parametersValues)), nullptr };
	}

}

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Calling function";
	auto executionContext = ExecutionContext{ node.parent, node.GetFunction() };
	SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Getting function in-memory context";
	auto inMemoryFunctionZone = m_interpreter.interpret(executionContext).asRvalue();

	auto& functionValue = inMemoryFunctionZone.object.as<TokenVariant>();
	if (std::holds_alternative<ExecutionContext>(functionValue)) {
		assert(inMemoryFunctionZone.memory != nullptr);
		auto functionExecutionContext = inMemoryFunctionZone.object.nodeval<ExecutionContext>();
		auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(functionExecutionContext);
		return InterpreterOperationFunctionCallScript(m_interpreter, functionExecutionContext.memory(), operateOnFunction, node);
	} else {
		assert(std::holds_alternative<BridgeMemory>(functionValue));
		auto bridgeCall = inMemoryFunctionZone.object.nodeval<BridgeMemory>();
		assert(bridgeCall != nullptr);
		auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(executionContext);
		return InterpreterOperationFunctionCallBridge(m_interpreter, *bridgeCall, operateOnFunction, node);
	}
}
