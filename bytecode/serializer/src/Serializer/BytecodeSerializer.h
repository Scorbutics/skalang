#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeSerializationStrategy.h"
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {

	namespace bytecode {
		struct DeserializationContext;
		struct SerializationContext;
		class ScriptCache;

		class Serializer {
		public:
			Serializer() = default;
			~Serializer() = default;

			bool serialize(SerializationContext& context) const;
			bool serialize(const ScriptCache& cache, SerializationStrategy output) const;
			std::vector<std::string> deserialize(DeserializationContext& output) const;
			std::vector<std::string> deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input) const;
		};
	}
}
