#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(OperateOn node) {
	auto inMemoryArrayZone = m_memory[node.GetArrayName()];
	auto& arrayCell = *inMemoryArrayZone.first->as< std::shared_ptr<std::vector<NodeValue>>>();
	auto arrayIndex = m_interpreter.interpret(node.GetArrayIndexNode()).asRvalue().nodeval<int>();
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return std::make_pair(&arrayCell[arrayIndex], inMemoryArrayZone.second);
}
