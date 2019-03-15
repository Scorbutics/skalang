#include "NodeValue/AST.h"
#include "NodeValue.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_DECLARATION>::interpret(OperateOn node) {
	auto result = std::make_shared<std::vector<NodeValue>>();
	for (auto& child : node) {
		result->push_back(std::move(m_interpreter.interpret({ node.parent, *child }).as<NodeValue>()));
	}
	return NodeCell {result};
}
