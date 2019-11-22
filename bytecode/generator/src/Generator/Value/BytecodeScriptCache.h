#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "Generator/Value/BytecodeScriptGenCache.h"

namespace ska {
  class Symbol;
	class ASTNode;
  namespace bytecode {
    struct ScriptGenerationService;

    class ScriptCache :
      public ScriptCacheBase<ScriptGenerationOutput> {
				using Parent = ScriptCacheBase<ScriptGenerationOutput>;
    public:
      using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;

			bool isGenerated(std::size_t index) const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			void setSymbolInfo(const Symbol& symbol, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;


			const std::vector<Value>& getExportedSymbols(std::size_t scriptIndex);

      ScriptGenCache genCache;

    private:
      SymbolInfosContainer m_symbolInfo;
    };
  }
}
