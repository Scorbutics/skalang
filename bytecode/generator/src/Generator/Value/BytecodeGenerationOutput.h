#pragma once
#include <vector>
#include <unordered_map>
#include "BytecodeStorage.h"
#include "BytecodeSymbolInfo.h"

namespace ska {
	class Symbol;
	class ASTNode;
	namespace bytecode {
		using ScriptGenerationNameToIndexMap = std::unordered_map<std::string, std::size_t>;

		class GenerationOutput {
			using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;
		public:
			GenerationOutput(BytecodeStorage& storage);

			std::size_t push(ScriptGenerationService service);

			void setOut(std::size_t index, ScriptGenerationOutput scriptOutput);

			GenerationOutput(const GenerationOutput&) = delete;
			GenerationOutput(GenerationOutput&&) = default;
			GenerationOutput& operator=(const GenerationOutput&) = delete;
			~GenerationOutput() = default;

			auto size() const { return m_storage.services.size(); }

			auto& backService() { return m_storage.services.back(); }
			const auto& generated() const { return m_storage.output; }
			auto& back() { return m_storage.output.back(); }
			const auto& back() const { return m_storage.output.back(); }

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			void setSymbolInfo(const Symbol& symbol, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;

			std::pair<std::size_t, ScriptGenerationService*> script(const std::string& fullScriptName);
			ScriptGenerationService& script(std::size_t index);

			const std::vector<Value>& generateExportedSymbols(std::size_t scriptIndex) const;

		private:
			BytecodeStorage& m_storage;
			ScriptGenerationNameToIndexMap m_mapping;
			SymbolInfosContainer m_symbolInfo;
		};
	}
}
