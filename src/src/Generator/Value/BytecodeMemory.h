#pragma once
#include <string>
#include <unordered_map>
#include "BytecodeValue.h"

namespace ska {
    namespace bytecode {
        using Memory = std::unordered_map<std::string, Value>;
    }
}
