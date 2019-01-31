#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Operation/Interpreter/OperationFunctionDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	auto inMemoryFunctionZone = m_interpreter.interpret(node.GetFunction()).asLvalue();
	assert(inMemoryFunctionZone.first != nullptr && inMemoryFunctionZone.second != nullptr);

	auto astFunction = inMemoryFunctionZone.first->nodeval<ASTNode*>();
	auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(*astFunction);

	//Centers memory on the current function scope
	auto& currentExecutionMemoryZone = m_memory.pointTo(*inMemoryFunctionZone.second);

	//Creates function-memory environment scope (including creation of parameters)
	m_memory.createNested();
	
	auto& functionPrototype = operateOnFunction.GetFunctionPrototype();
	for (auto index = 0u; index < functionPrototype.size() - 1 && node.HasFunctionParameter(index); index++) {
		auto& functionParameter = functionPrototype[index];
		
		auto nodeValue = m_interpreter.interpret(node.GetFunctionParameterValue(index)).asRvalue();
		m_memory.put(functionParameter.name(), std::move(nodeValue));
	}
	auto result = m_interpreter.interpret(operateOnFunction.GetFunctionBody());
	
	//Go back to the current execution scope, while destroying the memory used during function execution
	m_memory.popNested();
	m_memory.pointTo(currentExecutionMemoryZone);

	return result;
}
