#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_DECLARATION>::interpret(OperateOn node) {
	assert(!node.GetFunctionName().empty());
	auto* astPtr = &node.GetFunction();
	m_memory.put(node.GetFunctionName(), astPtr);
	return NodeCell {astPtr};
}
