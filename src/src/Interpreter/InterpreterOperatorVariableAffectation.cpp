#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableAffectation.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_AFFECTATION>::interpret(OperateOn node) {
	auto memCell = m_interpreter.interpret(node.GetVariableNameNode());
	memCell.asLvalue() = m_interpreter.interpret(node.GetVariableValueNode()).asRvalue();
	return memCell;
}
