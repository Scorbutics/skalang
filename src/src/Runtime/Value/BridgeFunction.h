#pragma once
#include <functional>
#include <memory>
#include "NodeValue/ASTNodePtr.h"

namespace ska {
	class NodeValue;

	struct BridgeFunction {
		using Callback = std::function<NodeValue(std::vector<NodeValue>)>;

		template <class F>
		BridgeFunction(F&& callback) : function(std::forward<F>(callback)) {}

		Callback function;
		ASTNodePtr node;
	};
}
