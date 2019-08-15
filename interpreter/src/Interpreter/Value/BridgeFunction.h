#pragma once
#include <functional>
#include <memory>
#include "NodeValue/ASTNodePtr.h"

namespace ska {
	class NodeValue;
	class Script;
	struct BridgeFunction {
        using Callback = std::function<NodeValue(Script& caller, std::vector<NodeValue>)>;
        
        template <class F>
        BridgeFunction(F&& callback) : function(std::forward<F>(callback)) {}

		Callback function;
		ASTNodePtr node;
	};
}
