#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(OperateOn node) {
	m_memory.createNested();
	auto output = NodeValue{};
	auto index = 0u;
	for (auto& child : node) {
		auto childCell = m_interpreter.interpret(*child);
		if(index == node.size() - 1) {
			output = childCell.asRvalue();
		}
		index++;
	}
	m_memory.endNested();
	return NodeCell{ std::move(output) };
}