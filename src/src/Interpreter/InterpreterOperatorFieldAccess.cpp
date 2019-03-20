#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	auto* objectMemoryZone = m_interpreter.interpret({ node.parent, node.GetObject() }).asLvalue().first;
	assert(objectMemoryZone != nullptr);
	auto& objectVariantMemory = objectMemoryZone->as<Token::Variant>();
	
	using MemoryZone = std::shared_ptr<MemoryTable>;
	using ScriptZone = ExecutionContext;
	using BridgingZone = std::shared_ptr<BridgeFunction>;

	if (std::holds_alternative<MemoryZone>(objectVariantMemory)) {
		auto& memoryObject = (*objectMemoryZone->nodeval<MemoryZone>());
		return memoryObject(node.GetFieldName());
	} else if (std::holds_alternative<BridgingZone>(objectVariantMemory)) {
		auto& bridgingZone = std::get<BridgingZone>(objectVariantMemory);
		//TODO handle several script functions
		return NodeCell{ node.parent.memory(), objectMemoryZone };
	} else {
		auto& scriptZone = std::get<ScriptZone>(objectVariantMemory);
		auto& memoryScript = scriptZone.program().memory().down();
		return memoryScript(node.GetFieldName());
	}
}
