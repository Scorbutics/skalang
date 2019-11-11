#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
  namespace bytecode {
    struct Script;

    struct ScriptCache : public ScriptCacheBase<Script> {
      ScriptCacheAST astCache;
    };
  }
}
