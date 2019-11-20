#pragma once
#include <vector>
#include <unordered_map>
#include "BytecodeSymbolInfo.h"
#include "BytecodeScriptGenerationService.h"
#include "BytecodeScriptGenerationOutput.h"

namespace ska {
	class Symbol;
	class ASTNode;

	namespace bytecode {
		struct ScriptCache;
		using ScriptGenerationNameToIndexMap = std::unordered_map<std::string, std::size_t>;

		class GenerationOutput {
			using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;
		public:
			GenerationOutput(ScriptCache& cache);

			std::size_t push(ScriptGenerationService service);

			void setOut(std::size_t index, ScriptGenerationOutput scriptOutput);

			GenerationOutput(const GenerationOutput&) = delete;
			GenerationOutput(GenerationOutput&&) = default;
			GenerationOutput& operator=(const GenerationOutput&) = delete;
			GenerationOutput& operator=(GenerationOutput&&) = default;
			~GenerationOutput() = default;

			std::size_t size() const;

			const ScriptGenerationOutput& generated(std::size_t index) const;
			bool isGenerated(std::size_t index) const;

			ScriptGenerationService& backService();
			ScriptGenerationOutput& back();
			const ScriptGenerationOutput& back() const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			void setSymbolInfo(const Symbol& symbol, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;

			std::pair<std::size_t, ScriptGenerationService*> script(const std::string& fullScriptName);
			ScriptGenerationService& script(std::size_t index);
			const ScriptGenerationService& script(std::size_t index) const;

			const std::vector<Value>& generateExportedSymbols(std::size_t scriptIndex) const;

		private:
			ScriptCache* m_cache = nullptr;
			SymbolInfosContainer m_symbolInfo;
		};
	}
}
