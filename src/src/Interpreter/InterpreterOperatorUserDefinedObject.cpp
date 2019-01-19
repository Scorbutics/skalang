#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUserDefinedObject.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::USER_DEFINED_OBJECT>::interpret(OperateOn node) {
	auto objectMemory = std::make_shared<MemoryTable>();
	auto& executionMemoryPoint = m_memory.pointTo(*objectMemory);
	for(auto& field: node) {
		m_interpreter.interpret(*field);
	}
	m_memory.pointTo(executionMemoryPoint);
	return objectMemory;
}
