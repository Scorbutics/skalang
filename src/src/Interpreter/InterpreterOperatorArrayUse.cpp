#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(OperateOn node) {
	auto inMemoryArrayZone = node.parent.findInMemoryTree(node.GetArrayName());
	auto& arrayCell = *inMemoryArrayZone.first->as<NodeValueArray>();
	auto arrayIndex = m_interpreter.interpret({ node.parent, node.GetArrayIndexNode() }).asRvalue().object.nodeval<int>();
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return NodeLValue{&arrayCell[arrayIndex], inMemoryArrayZone.second};
}
