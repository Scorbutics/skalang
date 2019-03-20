#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFor.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FOR_LOOP>::interpret(OperateOn node) {
	node.parent.memory().createNested();
	m_interpreter.interpret({ node.parent, node.GetInitialization() });
	auto condition = NodeValue {};
	while ((condition = m_interpreter.interpret({ node.parent, node.GetCondition() }).asRvalue()).nodeval<bool>()) {
		node.parent.memory().createNested();
		m_interpreter.interpret({ node.parent, node.GetStatement() });
		m_interpreter.interpret({ node.parent, node.GetIncrement() });
		node.parent.memory().endNested();
	}	
	node.parent.memory().endNested();
	return NodeCell {""};
}
