#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFor.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FOR_LOOP>::interpret(OperateOn node) {
	node.parent.pushNestedMemory();
	m_interpreter.interpret({ node.parent, node.GetInitialization() });
	auto condition = NodeValue {};
	while ((condition = m_interpreter.interpret({ node.parent, node.GetCondition() }).asRvalue().object).nodeval<bool>()) {
		node.parent.pushNestedMemory();
		m_interpreter.interpret({ node.parent, node.GetStatement() });
		m_interpreter.interpret({ node.parent, node.GetIncrement() });
		node.parent.endNestedMemory();
	}	
	node.parent.endNestedMemory();
	return {};
}
