#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include "Runtime/Value/SerializerOutput.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct Operand;
		class ScriptCache;
		class OperandSerializer {
		public:
			OperandSerializer() = default;
			~OperandSerializer() = default;

			static void write(const ScriptCache& cache, SerializerSafeZone<sizeof(uint8_t) + sizeof(Chunk) * 2> output, const Operand& value);

		private:
			static const std::string scriptName(const ScriptCache& cache, std::size_t id);
		};
	}
}