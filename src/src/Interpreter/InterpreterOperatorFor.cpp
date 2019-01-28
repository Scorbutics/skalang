#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFor.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FOR_LOOP>::interpret(OperateOn node) {
	m_memory.createNested();
	m_interpreter.interpret(node.GetInitialization());
	auto condition = NodeValue {};
	while ((condition = m_interpreter.interpret(node.GetCondition()).asRvalue()).nodeval<bool>()) {
		m_memory.createNested();
		m_interpreter.interpret(node.GetStatement());
		m_interpreter.interpret(node.GetIncrement());
		m_memory.endNested();
	}	
	m_memory.endNested();
	return NodeCell {""};
}
