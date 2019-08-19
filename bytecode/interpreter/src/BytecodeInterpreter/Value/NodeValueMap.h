#pragma once
#include <memory>
#include <unordered_map>
#include <string>

namespace ska {
    namespace bytecode {
        class NodeValue;
        using NodeValueMap = std::shared_ptr<std::unordered_map<std::string, NodeValue>>;
    }
}
