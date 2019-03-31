#pragma once
#include <memory>
#include <tuple>
#include <vector>

namespace ska {
	class NodeValue;
    using NodeValueArray = std::shared_ptr<std::vector<NodeValue>>;
}