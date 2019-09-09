#pragma once
#include <cassert>
#include "Value/BytecodeScript.h"
#include "Value/BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationContext {
		public:
			GenerationContext() = default;
			GenerationContext(ScriptGeneration& script);
			GenerationContext(ScriptGeneration& script, const ASTNode& node, std::size_t scopeLevel);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }
			ScriptGeneration& script() { assert(m_script != nullptr); return *m_script; }
			auto scope() const { return m_scopeLevel; }
		private:
			ScriptGeneration* m_script {};
			const ASTNode* m_pointer {};
			std::size_t m_scopeLevel = 0;
		};
	}
}
