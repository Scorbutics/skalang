#pragma once
#include <memory>
#include <vector>

namespace ska {
    class NodeValue;
    using NodeValueArray = std::shared_ptr<std::vector<NodeValue>>;
}