#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorIfElse.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::IF>::interpret(OperateOn node) {
	auto conditionValue = m_interpreter.interpret({ node.parent, node.GetCondition() }).asRvalue();
	if(conditionValue.nodeval<bool>()) {
		m_interpreter.interpret({ node.parent, node.GetIfStatement() });
    }
	
	return NodeCell {""};
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::IF_ELSE>::interpret(OperateOn node) {
	auto conditionValue = m_interpreter.interpret({ node.parent, node.GetCondition() }).asRvalue();
	if(conditionValue.nodeval<bool>()) {
		m_interpreter.interpret({ node.parent, node.GetIfStatement() });
    } else {
		m_interpreter.interpret({ node.parent, node.GetElseStatement() });
    }
	
	return NodeCell {""};
}
