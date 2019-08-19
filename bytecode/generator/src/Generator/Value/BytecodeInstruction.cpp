#include "BytecodeInstruction.h"

//#define LOG_TYPE
#define LOG_CMD
#define LOG_VALUE

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const Instruction& cell) {
	stream << "["
	#ifdef LOG_TYPE
	<< cell.m_type << "|"
	#endif
	#ifdef LOG_CMD
	<< CommandSTR[static_cast<std::size_t>(cell.m_command)] << "|"
	#endif
	#ifdef LOG_VALUE
	<< cell.m_dest.toString() << "|"
	<< cell.m_left.toString() << "|"
	<< cell.m_right.toString()
	#endif
	<< "]";
	return stream;
}
