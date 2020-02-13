#pragma once
#include <cstddef>

#include "BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct ScriptBody {
			std::vector<Instruction> instructions {};
			std::vector<Operand> exports {};
			std::vector<Chunk> linkedScriptsRef {};
		};
	}
}
