#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(OperateOn node) {
	auto array = m_interpreter.interpret({ node.parent, node.GetArrayNode() }).asLvalue();
	auto& arrayCell = *array.object->as<NodeValueArray>();//*inMemoryArrayZone.first->as<NodeValueArray>();
	auto arrayIndex = m_interpreter.interpret({ node.parent, node.GetArrayIndexNode() }).asRvalue().object.nodeval<int>();
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return NodeLValue{&arrayCell[arrayIndex], array.memory};
}
