#pragma once
#include <string>
#include <unordered_map>
#include "BytecodeValue.h"

namespace ska {
    using BytecodeMemory = std::unordered_map<std::string, BytecodeValue>;

    /*
    class BytecodeMemory {
    public:
        BytecodeMemory() = default;
        ~BytecodeMemory() = default;
    private:
        std::vector<BytecodeValue> m_values;
    };
    */
}
