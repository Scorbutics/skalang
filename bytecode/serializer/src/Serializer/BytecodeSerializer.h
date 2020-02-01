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
		private:
			void serialize(const Operand& operand, SerializationContext& output, std::vector<std::string>& natives) const;
		};
	}
}
