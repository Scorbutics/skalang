#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFor.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FOR_LOOP>::interpret(OperateOn node) {
	auto lock = node.parent.pushNestedMemory(true);
	m_interpreter.interpret({ node.parent, node.GetInitialization() });
	auto condition = NodeValue {};
	while ((condition = m_interpreter.interpret({ node.parent, node.GetCondition() }).asRvalue().object).nodeval<bool>()) {
		auto innerlock = node.parent.pushNestedMemory(true);
		if(!node.GetStatement().logicalEmpty()) {
			m_interpreter.interpret({ node.parent, node.GetStatement() });
		}
		if(!node.GetIncrement().logicalEmpty()) {
			m_interpreter.interpret({ node.parent, node.GetIncrement() });
		}
	}	
	return {};
}
