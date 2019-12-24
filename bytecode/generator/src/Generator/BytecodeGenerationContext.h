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
		class GeneratorOperatorUnit;
		class GenerationContext {
		public:
			GenerationContext(GenerationOutput& output, ScriptGenerationHelper script);
			GenerationContext(GenerationOutput& output, const ScriptAST& scriptAst);
			GenerationContext(GenerationContext& old);
			GenerationContext(GenerationContext& old, const ScriptAST& scriptAst);
			GenerationContext(GenerationContext& old, ScriptGenerationHelper script);

			GenerationContext(GenerationContext&& mv) = default;

			GenerationContext(GenerationContext& old, const ASTNode& node, std::size_t scopeLevelOffset = 0);

			const ASTNode& pointer() const { assert(m_pointer != nullptr); return *m_pointer; }

			auto scope() const { return m_scopeLevel; }

			std::pair<std::size_t, ScriptGeneration*> script(const std::string& fullScriptName);
			std::size_t totalScripts() const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			Register queryNextRegister();
			ScriptASTPtr useImport(const std::string& scriptImported);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;
			Operand querySymbolOrOperand(const ASTNode& node);
			Operand querySymbol(const Symbol& symbol);
			std::optional<Operand> getSymbol(const Symbol& symbol) const;

			const auto scriptIndex() const { return m_script.id(); }
			auto scriptName() const { return m_script.name(); }

		private:
			ScriptGenerationHelper& helper();
			const ScriptGenerationHelper& helper() const;
			ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol);
			const ScriptGenerationHelper& scriptOfSymbol(const Symbol& symbol) const;

			GenerationOutput& m_generated;
			ScriptGeneration& m_script;
			const ASTNode* m_pointer {};
			std::size_t m_scopeLevel = 0;
		};
	}
}
