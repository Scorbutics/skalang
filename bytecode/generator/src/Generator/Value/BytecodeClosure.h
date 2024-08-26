#pragma once
#include <unordered_set>
#include "BytecodeInstructionOutput.h"

namespace ska {
	class ScopedSymbolTable;
	class ASTNode;
	namespace bytecode {
		class GenerationContext;

		class Closure {
		public:
			Closure() = default;
			Closure(const ASTNode& scopeNode);

			InstructionOutput checkAndCapture(GenerationContext& context, const ScopedSymbolTable* variable, Operand variableOperand);
			InstructionOutput generate(const GenerationContext& context) const;

		private:
			const ASTNode* m_scopeNode = nullptr;
			std::unordered_set<const ScopedSymbolTable*> m_environment;
			InstructionOutput m_usageEnvironment;
		};
	}
}