#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScriptGenerationService.h"

namespace ska {
  namespace bytecode {
    struct Script;

    struct ScriptCache : public ScriptCacheBase<Script> {
      ScriptCacheAST astCache;
    };
  }
}
