#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionCall.h"
#include "Operation/OperationFunctionDeclaration.h"
ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_CALL>::interpret(OperateOn node) {
	const auto* inMemoryFunction = m_memory[node.GetFunctionName()];
	assert(inMemoryFunction != nullptr);

	//TODO add parameters passing
	auto operateOnFunction = Operation<Operator::FUNCTION_DECLARATION>(*inMemoryFunction->nodeval<ASTNode*>());
	auto result = m_interpreter.interpret(operateOnFunction.GetFunctionBody());

	return result;
}
