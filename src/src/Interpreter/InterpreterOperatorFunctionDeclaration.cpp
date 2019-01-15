#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_DECLARATION>::interpret(OperateOn node) {
	assert(!node.GetFunctionName().empty());
	m_memory.put(node.GetFunctionName(), &node.GetFunction());
	return "";
}
