#pragma once
#include <functional>
#include <memory>
#include "NodeValue.h"
#include "NodeValue/ASTNodePtr.h"

namespace ska {
	struct BridgeFunction {
        using Callback = std::function<NodeValue(std::vector<NodeValue>)>;
        
        template <class F>
        BridgeFunction(F&& callback) : function(std::forward<F>(callback)) {}

		Callback function;
		ASTNodePtr node;
	};
}
