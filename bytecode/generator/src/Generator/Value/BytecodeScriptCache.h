#pragma once
#include "BytecodeGenerationOutput.h"
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScriptGenerationService.h"

namespace ska {
  namespace bytecode {
    struct ScriptGenerationService;

    struct ScriptCacheUnit {
      ScriptGenerationService generation;
      ScriptGenerationOutput execution;
    };

    struct ScriptCache : public ScriptCacheBase<ScriptCacheUnit> {
      ScriptCache() : output(*this) {}
      ScriptCache(ScriptCache&&) = default;
      ScriptCache(const ScriptCache&) = delete;
      ScriptCache& operator=(ScriptCache&&) = default;
      ScriptCache& operator=(const ScriptCache&) = delete;
      ~ScriptCache() = default;
      GenerationOutput output;
      ScriptCacheAST astCache;
    };
  }
}
