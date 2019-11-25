#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationOutput.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGenerationOutput& output) {
	stream << output.m_generated;
	return stream;
}
