#pragma once
#include <cstddef>

#include "BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader {
			Chunk scriptNameRef { 0 };
			std::string scriptName {};
			std::size_t serializerVersion { 0 };
			std::size_t scriptBridged { 0 };
			std::size_t scriptId { 0 };
		};
	}
}
