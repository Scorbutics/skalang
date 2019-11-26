#pragma once
#include <cassert>
#include <tuple>
#include "Value/BytecodeOperand.h"
#include "Value/BytecodeSymbolInfo.h"
#include "Value/BytecodeScriptGenerationHelper.h"
#include "Value/BytecodeGenerationOutput.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class GenerationContext {
		public:
			explicit GenerationContext(GenerationOutput& output, std::size_t scriptIndex = std::numeric_limits<std::size_t>::max());
			GenerationContext(GenerationContext& old);
			GenerationContext(GenerationContext& old, ScriptGenerationHelper script);

			GenerationContext(GenerationContext&& mv) = default;

			GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset = 0);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }
			ScriptGenerationHelper& script();
			const ScriptGenerationHelper& script() const;
			auto scope() const { return m_scopeLevel; }

			std::pair<std::size_t, ScriptGenerationHelper*> script(const std::string& fullScriptName);
			std::size_t totalScripts() const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;
			Operand querySymbolOrOperand(const ASTNode& node);
			Operand querySymbol(const Symbol& symbol);
			std::optional<Operand> getSymbol(const Symbol& symbol) const;

			const auto scriptIndex() const { return m_scriptIndex; }

		private:
			ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol);
			const ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol) const;

			GenerationOutput& m_generated;
			std::size_t m_scriptIndex = 0;
			const ASTNode* m_pointer {};
			std::size_t m_scopeLevel = 0;
		};
	}
}
