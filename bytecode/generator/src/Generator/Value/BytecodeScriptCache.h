#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
  namespace bytecode {
    struct Storage {
      ScriptGenerationService service;
      ScriptGenerationOutput output;
    };

    using ScriptCache = ScriptCacheBase<Storage>;
  }
}
