#pragma once
#include "BytecodeScriptGenerationHelper.h"

namespace ska {
  class Symbol;
	class ASTNode;
  namespace bytecode {
    struct ScriptGenerationHelper;

    class ScriptGenCache :
      public ScriptCacheBase<ScriptGenerationHelper> {
				using Parent = ScriptCacheBase<ScriptGenerationHelper>;
    public:
      using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;

			std::pair<std::size_t, ScriptGenerationHelper*> get(const std::string& fullScriptName);
			ScriptGenerationHelper& get(std::size_t index);
			const ScriptGenerationHelper& get(std::size_t index) const;

      ScriptCacheAST astCache;
    };
  }
}
