#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(OperateOn node) {
	node.parent.memory().createNested();
	auto output = NodeValue{};
	auto index = 0u;
	for (auto& child : node) {
		auto childCell = m_interpreter.interpret({ node.parent, *child });
		if(index == node.size() - 1) {
			output = childCell.asRvalue();
		}
		index++;
	}
	node.parent.memory().endNested();
	return NodeCell{ std::move(output) };
}