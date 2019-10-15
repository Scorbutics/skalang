#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUserDefinedObject.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::USER_DEFINED_OBJECT>::interpret(OperateOn node) {
	auto objectMemory = node.parent.createMemory();
	auto executionMemoryPoint = node.parent.pointMemoryTo(objectMemory);
	for(auto& field: node) {
		m_interpreter.interpret({ node.parent, *field });
	}
	node.parent.pointMemoryTo(executionMemoryPoint);
	return NodeRValue{ objectMemory, objectMemory };
}
