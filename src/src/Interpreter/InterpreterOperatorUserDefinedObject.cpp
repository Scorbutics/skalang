#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUserDefinedObject.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::USER_DEFINED_OBJECT>::interpret(OperateOn node) {
	auto objectMemory = std::make_shared<MemoryTable>(node.parent.memory());
	auto& executionMemoryPoint = node.parent.memory().pointTo(*objectMemory);
	for(auto& field: node) {
		m_interpreter.interpret({ node.parent, *field });
	}
	node.parent.memory().pointTo(executionMemoryPoint);
	return NodeCell {objectMemory};
}
