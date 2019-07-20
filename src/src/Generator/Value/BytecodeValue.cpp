#include "NodeValue/AST.h"
#include "BytecodeValue.h"

ska::BytecodeRValue::BytecodeRValue(BytecodeCommand command, const ASTNode& node) :
	m_command(std::move(command)),
	m_value{ node.name(), node.tokenType(), node.positionInScript() },
	m_type(node.type().value()) {
}
