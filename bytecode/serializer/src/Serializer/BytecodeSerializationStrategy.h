#pragma once
#include <functional>
#include <fstream>

namespace ska {
    namespace bytecode {
        using DeserializationStrategy = std::function<std::ifstream&(const std::string&)>;
        using SerializationStrategy = std::function<std::ofstream&(const std::string&)>;

        struct DeserializationStrategyType {
            static DeserializationStrategy PerScript();
        };

        struct SerializationStrategyType {
            static SerializationStrategy PerScript();
        };
    }
}
