#include "NodeValue/AST.h"
#include "Runtime/Value/NodeValue.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_DECLARATION>::interpret(OperateOn node) {
	auto result = std::make_shared<std::deque<NodeValue>>();
	for (auto& child : node) {
		result->push_back(std::move(m_interpreter.interpret({ node.parent, *child }).asRvalue().object));
	}
	return NodeRValue{ std::move(result), nullptr };
}