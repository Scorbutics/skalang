#pragma once
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
  namespace bytecode {
    using ScriptGenerationOutputContainer = std::vector<ScriptGenerationOutput>;
		using ScriptGenerationServiceContainer = std::vector<ScriptGenerationService>;

    struct BytecodeStorage {
      ScriptGenerationServiceContainer services;
			ScriptGenerationOutputContainer output;
    };
  }
}
