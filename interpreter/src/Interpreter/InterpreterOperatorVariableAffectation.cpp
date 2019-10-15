#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableAffectation.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_AFFECTATION>::interpret(OperateOn node) {
	auto memCell = m_interpreter.interpret({ node.parent, node.GetVariableNameNode() });
	auto memCellLValue = memCell.asLvalue();
	auto value = m_interpreter.interpret({ node.parent, node.GetVariableValueNode() }).asRvalue();
	memCellLValue.memory = std::move(value.memory);
	*memCellLValue.object = std::move(value.object);
	return memCell;
}
