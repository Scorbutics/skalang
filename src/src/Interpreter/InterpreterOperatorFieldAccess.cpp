#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"
#include "Service/Script.h"
#include "NodeValue/ObjectMemory.h"

namespace ska {
	NodeCell InterpreterOperatorFieldAccessGetMemoryField(NodeValue& objectMemoryZone, const std::string& fieldName, bool isLvalue) {
		auto& objectVariantMemory = objectMemoryZone.as<Token::Variant>();

		if (std::holds_alternative<ObjectMemory>(objectVariantMemory)) {
			auto& memoryObject = objectMemoryZone.nodeval<ObjectMemory>();
			auto& memoryField = (*memoryObject)(fieldName);
			return isLvalue ? memoryField : NodeCell{ std::move(memoryField) };
		} else {
			auto& scriptZone = std::get<ExecutionContext>(objectVariantMemory);
			auto& memoryScript = scriptZone.program().memory().down();
			auto memoryField = (memoryScript)(fieldName);
			return isLvalue ? memoryField : NodeCell{ std::move(memoryField) };
		}
	}
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	auto object = m_interpreter.interpret({ node.parent, node.GetObject() });
	
	if (object.isLvalue()) {
		auto& objectMemoryZone = *object.asLvalue().first;
		return InterpreterOperatorFieldAccessGetMemoryField(objectMemoryZone, node.GetFieldName(), object.isLvalue());
	}

	auto objectMemoryZone = object.asRvalue();
	return InterpreterOperatorFieldAccessGetMemoryField(objectMemoryZone, node.GetFieldName(), object.isLvalue());
}
