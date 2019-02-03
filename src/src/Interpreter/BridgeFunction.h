#pragma once
#include <functional>
#include <memory>
#include "NodeValue.h"

namespace ska {
	struct BridgeFunction {
        using Callback = std::function<NodeValue(std::vector<NodeValue>)>;
        
        template <class F>
        BridgeFunction(F&& callback) : function(std::forward<F>(callback)) {}

		Callback function;
	};

	using BridgeFunctionPtr = std::shared_ptr<BridgeFunction>;
}
