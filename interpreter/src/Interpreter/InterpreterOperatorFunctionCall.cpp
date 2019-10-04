#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>)

namespace ska {

	std::vector<ska::NodeValue> InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(
		ska::Interpreter& interpreter,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {

		SLOG_STATIC(ska::LogLevel::Debug, ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>) << "Computing parameters value";

		auto result = std::vector<ska::NodeValue>{};
		auto& functionPrototype = operateOnFunctionDeclaration.GetFunctionPrototype();
		for (auto index = 0u; index < functionPrototype.size() - 1 && operateOnFunctionCall.HasFunctionParameter(index); index++) {
			LOG_DEBUG << "Building parameter " << index << "...";
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

		LOG_DEBUG << "Creating nested memory context to execute function";

		//Creates function-memory environment scope (including creation of parameters)
		auto lock = parentScript.pushNestedMemory(true);

		LOG_DEBUG << "Setting parameters values";

		for (auto& parameterValue : parametersValues) {
			auto& functionParameter = functionPrototype[index++];
			parentScript.putMemory(functionParameter.name(), std::move(parameterValue));
		}
		auto result = interpreter.interpret({ parentScript, operateOnFunctionDeclaration.GetFunctionBody() });

		LOG_DEBUG << "Poping nested memory and going back to call context";

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
		LOG_DEBUG << "Function is a script, transfering parameters";
		return InterpreterOperationFunctionCallScriptWithParams(node.parent, interpreter, memoryForFunctionExecution, operateOnFunctionDeclaration, std::move(parametersValues));
	}

	ska::NodeCell InterpreterOperationFunctionCallBridge(
		ska::Interpreter& interpreter,
		BridgeFunction& bridgeCall,
		ska::Operation<ska::Operator::FUNCTION_DECLARATION>& operateOnFunctionDeclaration,
		ska::Operation<ska::Operator::FUNCTION_CALL>& operateOnFunctionCall) {
		auto parametersValues = InterpreterOperationFunctionCallExecuteFunctionFromCallParameters(interpreter, operateOnFunctionDeclaration, operateOnFunctionCall);
		LOG_DEBUG << "Function is a bridge, transfering parameters";
		return NodeRValue{(bridgeCall.function)(std::move(parametersValues)), nullptr };
	}

}

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	LOG_DEBUG << "Calling function";
	auto executionContext = ExecutionContext{ node.parent, node.GetFunction() };
	LOG_DEBUG << "Getting function in-memory context";
	auto inMemoryFunctionZone = m_interpreter.interpret(executionContext).asRvalue();

	auto& functionValue = inMemoryFunctionZone.object.as<TokenVariant>();
	if (std::holds_alternative<ScriptVariableRef>(functionValue)) {
		assert(inMemoryFunctionZone.memory != nullptr);
		auto targetScriptId = inMemoryFunctionZone.object.nodeval<ScriptVariableRef>();
		auto targetScript = node.parent.useImport(targetScriptId.script);
		if(targetScript == nullptr) {
			throw std::runtime_error("bad script");
		}
		auto targetFunction = targetScript->getFunction(targetScriptId.variable);
		LOG_DEBUG << "Use function number " << targetScriptId.variable << " named " << targetFunction->node->name() << " declared in script " << targetScript->name() << " but called in script " << node.parent.name();
		auto functionExecutionContext = ExecutionContext { *targetScript, *targetFunction->node, targetFunction->memory };

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
