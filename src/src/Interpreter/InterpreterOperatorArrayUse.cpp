#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(OperateOn node) {
	auto& arrayCell = *m_memory[node.GetArrayName()]->as< std::shared_ptr<std::vector<NodeValue>>>();
	auto arrayIndex = m_interpreter.interpret(node.GetArrayIndexNode()).asRvalue().nodeval<int>();
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return &arrayCell[arrayIndex];
}
