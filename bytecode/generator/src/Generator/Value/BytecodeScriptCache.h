#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScriptGenerationService.h"

namespace ska {
  namespace bytecode {
    struct ScriptGen;

    struct ScriptCache : public ScriptCacheBase<ScriptGen> {
      ScriptCacheAST astCache;
    };
  }
}
