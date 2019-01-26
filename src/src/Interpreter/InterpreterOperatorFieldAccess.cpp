#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	const auto* objectMemoryZone = m_interpreter.interpret(node.GetObject()).asLvalue().first;
	assert(objectMemoryZone != nullptr);
	auto& objectMemory = *objectMemoryZone->nodeval<std::shared_ptr<MemoryTable>>();
	return objectMemory(node.GetFieldName());
}
