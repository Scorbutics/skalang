#pragma once
#include "BytecodeScriptGeneration.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Runtime/Value/NativeFunction.h"

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
      SymbolInfo getSymbolInfoOrNew(std::size_t scriptIndex, const Symbol& symbol) const;

			const std::vector<Operand>& getExportedSymbols(std::size_t scriptIndex);

      std::size_t storeBinding(NativeFunctionPtr binding);
      const NativeFunction& getBinding(std::size_t index) const;

      ScriptCacheAST astCache;

    private:
      SymbolInfosContainer m_symbolInfo;
      std::vector<NativeFunctionPtr> m_bindings;
    };
  }
}
