#pragma once
#include <unordered_set>
#include "Value/BytecodeSymbolInfo.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationOutput;
		class Instruction;
		class Value;

		class ObjectFieldAccessReplacer {
		public:
			void process(GenerationOutput& generated);
		private:
			void markFields(std::unordered_set<std::size_t>& valuesToWatch, const Value& valueToWatch, FieldsReferences fields);
		};
  }
}
