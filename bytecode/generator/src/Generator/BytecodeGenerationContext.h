#pragma once
#include <cassert>
#include <tuple>
#include "Value/BytecodeValue.h"
#include "Value/BytecodeSymbolInfo.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationOutput;
		class ScriptGenerationService;
		class GenerationContext {
		public:
			explicit GenerationContext(GenerationOutput& output);
			GenerationContext(GenerationContext& old);
			GenerationContext(GenerationContext& old, ScriptGenerationService script);

			GenerationContext(GenerationContext&& mv) = default;
			GenerationContext& operator=(GenerationContext&& mv) = default;

			GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset = 0);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }
			ScriptGenerationService& script() { assert(m_script != nullptr); return *m_script; }
			auto scope() const { return m_scopeLevel; }

			std::pair<std::size_t, ScriptGenerationService*> script(const std::string& fullScriptName);

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;

		private:
			GenerationOutput& m_generated;
			ScriptGenerationService* m_script {};
			std::size_t m_scriptIndex = 0;
			const ASTNode* m_pointer {};
			std::size_t m_scopeLevel = 0;
		};
	}
}
