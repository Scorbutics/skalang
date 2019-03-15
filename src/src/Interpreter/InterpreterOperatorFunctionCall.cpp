#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Operation/Interpreter/OperationFunctionDeclaration.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	auto inMemoryFunctionZone = m_interpreter.interpret({ node.parent, node.GetFunction() }).asLvalue();
	assert(inMemoryFunctionZone.first != nullptr && inMemoryFunctionZone.second != nullptr);

	auto astFunction = inMemoryFunctionZone.first->nodeval<ASTNode*>();
	auto executionContext = ExecutionContext{ node.parent, *astFunction };
	auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(executionContext);

	//Centers memory on the current function scope
	auto& currentExecutionMemoryZone = node.parent.memory().pointTo(*inMemoryFunctionZone.second);

	//Creates function-memory environment scope (including creation of parameters)
	node.parent.memory().createNested();
	
	auto& functionPrototype = operateOnFunction.GetFunctionPrototype();
	for (auto index = 0u; index < functionPrototype.size() - 1 && node.HasFunctionParameter(index); index++) {
		auto& functionParameter = functionPrototype[index];
		
		auto nodeValue = m_interpreter.interpret({ node.parent, node.GetFunctionParameterValue(index) }).asRvalue();
		node.parent.memory().put(functionParameter.name(), std::move(nodeValue));
	}
	auto result = m_interpreter.interpret({ node.parent, operateOnFunction.GetFunctionBody() });
	
	//Go back to the current execution scope, while destroying the memory used during function execution
	node.parent.memory().popNested();
	node.parent.memory().pointTo(currentExecutionMemoryZone);

	return result;
}
