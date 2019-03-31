#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableDeclaration.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_DECLARATION>::interpret(OperateOn node) {
	auto nodeValue = m_interpreter.interpret({ node.parent, node.GetVariableValueNode() }).asRvalue();
	node.parent.emplaceMemory(node.GetVariableName(), std::move(nodeValue.object));
	return {};
}
