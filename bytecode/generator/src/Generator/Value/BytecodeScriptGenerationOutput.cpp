#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGenerationOutput.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionPack& group) {
	for(const auto& c : group) {
		stream << c << "\n";
	}
	return stream;
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGenerationOutput& output) {
	stream << output.m_generated;
	return stream;
}
