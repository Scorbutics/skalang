#pragma once
#include <memory>
#include <tuple>
#include <deque>

namespace ska {
    namespace bytecode {
        class NodeValue;
        using NodeValueArray = std::shared_ptr<std::deque<NodeValue>>;
    }
}
