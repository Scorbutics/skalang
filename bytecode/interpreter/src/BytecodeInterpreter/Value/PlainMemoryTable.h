#pragma once

#include <vector>
#include "Runtime/Value/NodeValue.h"

namespace ska {
  namespace bytecode {
    using PlainMemoryTable = std::vector<NodeValue>;

    /*
    class PlainMemoryTable {
      public:

      private:
      std::vector<NodeValue> m_data;
    };
    */
  }
}
