#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGeneration.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGeneration& output) {
	stream << output.m_generated;
	return stream;
}
