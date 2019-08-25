#pragma once
#include <vector>

namespace ska {
  namespace bytecode {
    class GenerationOutput;

    class LabelReplacer {
      using LabelContainer = std::vector<std::size_t>;
    public:
      void process(GenerationOutput& generated);
    private:
      static void insertLabel(LabelContainer& container, std::size_t variableRef, std::size_t generationIndex);
    };
  }
}