#include "BytecodeInstruction.h"

#define LOG_CMD
#define LOG_VALUE

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const Instruction& cell) {
	stream << "["
	#ifdef LOG_CMD
	<< cell.m_command << "|"
	#endif
	#ifdef LOG_VALUE
	<< cell.m_dest.toString() << "|"
	<< cell.m_left.toString() << "|"
	<< cell.m_right.toString()
	#endif
	<< "]";
	return stream;
}
