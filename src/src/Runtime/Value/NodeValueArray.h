#pragma once
#include <memory>
#include <tuple>
#include <deque>

namespace ska {
	class NodeValue;
	using NodeValueArrayRaw = std::deque<NodeValue>;
	using NodeValueArray = std::shared_ptr<NodeValueArrayRaw>;
}
