#include "Config/LoggerConfigLang.h"
#include "BytecodeGenerationOutput.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionPack& group) {
		for(const auto& c : group) {
			stream << c << " ";
		}
}
