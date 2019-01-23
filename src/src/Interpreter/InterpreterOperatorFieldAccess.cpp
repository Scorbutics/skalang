#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	auto& objectMemory = *m_memory[node.GetObjectName()]->nodeval<std::shared_ptr<MemoryTable>>();
	return objectMemory(node.GetFieldName());
}
