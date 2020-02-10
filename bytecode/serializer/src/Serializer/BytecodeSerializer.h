#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeSerializationContext.h"

namespace ska {

	namespace bytecode {

		class Serializer {
		public:
			Serializer() = default;
			~Serializer() = default;

			void serialize(SerializationContext& context) const;
			void serialize(const ScriptCache& cache, std::ostream& output) const;
			void deserialize(DeserializationContext& output) const;
			void deserialize(ScriptCache& cache, std::istream& input) const;
		private:
			void replaceAllNativesRef(std::vector<Operand>& operands, const std::vector<std::string>& natives) const;
			void replaceAllNativesRef(std::vector<Instruction>& instructions, const std::vector<std::string>& natives) const;
			void replaceAllNativesRef(Operand& operand, const std::vector<std::string>& natives) const;
		};
	}
}
