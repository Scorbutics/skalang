#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(OperateOn node) {
	auto& arrayCell = *std::get<std::shared_ptr<std::vector<NodeValue>>>(*m_memory[node.GetArrayName()]);
	auto arrayIndex = nodeval<int>(m_interpreter.interpret(node.GetArrayIndexNode()).asRvalue());
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return &arrayCell[arrayIndex];
}
