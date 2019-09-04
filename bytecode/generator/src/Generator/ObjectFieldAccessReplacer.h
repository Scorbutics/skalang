#pragma once
#include <unordered_set>
#include "Value/BytecodeSymbolInfo.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationOutput;
		class Instruction;
		class Value;

		using ValueWatcher = std::unordered_set<Value>;

		class ObjectFieldAccessReplacer {
		public:
			void process(GenerationOutput& generated);
		};
  }
}
