#pragma once

namespace ska {
  namespace bytecode {
    class GenerationOutput;

    class SymbolInfoAdder {
    public:
      void process(GenerationOutput& generated);
    };
  }
}
