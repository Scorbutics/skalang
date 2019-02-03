#pragma once
#include <functional>
#include <memory>
#include "NodeValue.h"

namespace ska {
	struct BridgeFunction {
		std::function<NodeValue(std::vector<NodeValue>)> function;
	};

	using BridgeFunctionPtr = std::shared_ptr<BridgeFunction>;
}
