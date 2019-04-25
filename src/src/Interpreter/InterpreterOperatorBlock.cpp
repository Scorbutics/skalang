#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(OperateOn node) {
	auto lock = node.parent.pushNestedMemory(false);
	
	NodeRValue output;
	auto index = 0u;
	for (auto& child : node) {
		auto childCell = m_interpreter.interpret({ node.parent, *child });
		if(index == node.size() - 1) {
			output = childCell.asRvalue();
		}
		index++;
	}
	return std::move(output);
}