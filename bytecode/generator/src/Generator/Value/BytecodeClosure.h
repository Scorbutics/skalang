#pragma once
#include <unordered_set>
#include "BytecodeInstructionOutput.h"
#include "Base/Containers/insertion_indexed_map.h"

namespace ska {
	class ScopedSymbolTable;
	class ASTNode;
	namespace bytecode {
		class GenerationContext;

		using EnvironmentContainer = insertion_indexed_map<const ScopedSymbolTable*, const ScopedSymbolTable*>;

		class Closure {
		public:
			Closure() = default;
			Closure(const ASTNode& scopeNode);

			InstructionOutput checkAndCapture(GenerationContext& context, const ScopedSymbolTable* variable, Operand variableOperand);
			InstructionOutput generate(const GenerationContext& context) const;

		private:
			const ASTNode* m_scopeNode = nullptr;
			EnvironmentContainer m_environment;
			InstructionOutput m_usageEnvironment;
		};
	}
}
