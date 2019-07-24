#include "NodeValue/AST.h"
#include "BytecodeValue.h"

//#define LOG_TYPE
#define LOG_CMD
#define LOG_VALUE

ska::BytecodeRValue::BytecodeRValue(BytecodeCommand command, const ASTNode& node) :
	m_command(std::move(command)),
	m_value{ node.name(), node.tokenType(), node.positionInScript() },
	m_type(node.type().value()) {
}

std::ostream& ska::operator<<(std::ostream& stream, const ska::BytecodeRValue& cell) {
		
		stream << "[" 
		#ifdef LOG_TYPE
		<< cell.m_type << "|" 
		#endif
		#ifdef LOG_CMD
		<< BytecodeCommandSTR[static_cast<std::size_t>(cell.m_command)] << "|" 
		#endif
		#ifdef LOG_VALUE
		<< cell.m_value.name() 
		#endif
		<< "]";
}
