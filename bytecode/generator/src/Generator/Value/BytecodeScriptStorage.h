#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
  namespace bytecode {
    //using ScriptGenerationOutputContainer = std::vector<ScriptGenerationOutput>;
		//using ScriptGenerationServiceContainer = std::vector<ScriptGenerationService>;

    struct Storage {
      ScriptGenerationService service;
      ScriptGenerationOutput output;
    };

    using ScriptStorage = ScriptCacheBase<Storage>;

    /*
    struct ScriptStorage {
      ScriptGenerationServiceContainer services;
			ScriptGenerationOutputContainer output;
    };
    */
  }
}
