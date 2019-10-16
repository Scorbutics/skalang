#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
  namespace bytecode {
    struct Storage;

    struct ScriptCache : public ScriptCacheBase<Storage> {
      ScriptCacheAST astCache;
    };
  }
}
