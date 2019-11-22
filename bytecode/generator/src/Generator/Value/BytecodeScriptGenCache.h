#pragma once
#include "BytecodeScriptGenerationService.h"

namespace ska {
  class Symbol;
	class ASTNode;
  namespace bytecode {
    struct ScriptGenerationService;

    class ScriptGenCache :
      public ScriptCacheBase<ScriptGenerationService> {
				using Parent = ScriptCacheBase<ScriptGenerationService>;
    public:
      using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;

			std::pair<std::size_t, ScriptGenerationService*> get(const std::string& fullScriptName);
			ScriptGenerationService& get(std::size_t index);
			const ScriptGenerationService& get(std::size_t index) const;

      ScriptCacheAST astCache;
    };
  }
}
