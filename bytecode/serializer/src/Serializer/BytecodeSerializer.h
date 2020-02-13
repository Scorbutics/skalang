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
			bool deserialize(DeserializationContext& output) const;
			bool deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input) const;
		private:
			void replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const;
			void replaceAllNativesRef(std::vector<Instruction>& instructions, const std::vector<std::string>& natives) const;
			void replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const;
		};
	}
}
