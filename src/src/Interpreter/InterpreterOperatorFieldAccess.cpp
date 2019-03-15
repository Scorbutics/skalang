#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	const auto* objectMemoryZone = m_interpreter.interpret({ node.parent, node.GetObject() }).asLvalue().first;
	assert(objectMemoryZone != nullptr);
	auto& objectVariantMemory = objectMemoryZone->as<Token::Variant>();
	using MemoryZone = std::shared_ptr<MemoryTable>;
	using ScriptZone = ASTNode*;
	if (std::holds_alternative<MemoryZone>(objectVariantMemory)) {
		//std::get<MemoryZone>(objectVariantMemory);
	} else {
		//std::get<ScriptZone>(objectVariantMemory);
	}
	auto& objectMemory = *objectMemoryZone->nodeval<MemoryZone>();
	return objectMemory(node.GetFieldName());
}
