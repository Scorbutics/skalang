#pragma once
#include <cassert>
#include "Value/BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationOutput;
		class ScriptGenerationService;
		class GenerationContext {
		public:
			explicit GenerationContext(GenerationOutput& output);
			GenerationContext(GenerationContext& old);

			GenerationContext(GenerationContext&& mv) = default;
			GenerationContext& operator=(GenerationContext&& mv) = default;

			GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset = 0);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }
			ScriptGenerationService& script() { assert(m_script != nullptr); return *m_script; }
			auto scope() const { return m_scopeLevel; }
		private:
			GenerationOutput& m_generated;
			ScriptGenerationService* m_script {};
			const ASTNode* m_pointer {};
			std::size_t m_scopeLevel = 0;
		};
	}
}
