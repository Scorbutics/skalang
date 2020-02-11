#pragma once
#include <functional>
#include <fstream>

namespace ska {
    namespace bytecode {
        using DeserializationStrategy = std::function<std::ifstream&(std::size_t)>;
        using SerializationStrategy = std::function<std::ofstream&(std::size_t)>;

        struct DeserializationStrategyType {
            static DeserializationStrategy PerScript();
        };

        struct SerializationStrategyType {
            static SerializationStrategy PerScript();
        };
    }
}
