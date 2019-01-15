#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_DECLARATION>::interpret(OperateOn node) {
	auto nodeValue = m_interpreter.interpret(node.GetVariableValueNode).asRvalue();
	m_memory.put(node.GetVariableName(), std::move(nodeValue));
	return "";
}
