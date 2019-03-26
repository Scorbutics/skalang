#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"
#include "Service/Script.h"
#include "NodeValue/ObjectMemory.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	auto* objectMemoryZone = m_interpreter.interpret({ node.parent, node.GetObject() }).asLvalue().first;
	assert(objectMemoryZone != nullptr);
	auto& objectVariantMemory = objectMemoryZone->as<Token::Variant>();

	if (std::holds_alternative<ObjectMemory>(objectVariantMemory)) {
		auto& memoryObject = (*objectMemoryZone->nodeval<ObjectMemory>());
		return memoryObject(node.GetFieldName());
	} else {
		auto& scriptZone = std::get<ExecutionContext>(objectVariantMemory);
		auto& memoryScript = scriptZone.program().memory().down();
		return memoryScript(node.GetFieldName());
	}
}
