#pragma once
#include "BytecodeScriptGeneration.h"
#include "NodeValue/ScriptCacheAST.h"
namespace ska {
  class Symbol;
	class ASTNode;
  namespace bytecode {
    class ScriptCache :
      public ScriptCacheBase<ScriptGeneration> {
				using Parent = ScriptCacheBase<ScriptGeneration>;
    public:
      using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;

			bool isGenerated(std::size_t index) const;

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			void setSymbolInfo(const Symbol& symbol, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;


			const std::vector<Operand>& getExportedSymbols(std::size_t scriptIndex);

      ScriptCacheAST astCache;

    private:
      SymbolInfosContainer m_symbolInfo;
    };
  }
}
