#pragma once
#include <functional>
#include <memory>
#include "NodeValue/ASTNodePtr.h"

namespace ska {
	class NodeValue;

	struct NativeFunction {
		using Callback = std::function<NodeValue(std::vector<NodeValue>)>;

		template <class F>
		NativeFunction(F&& callback) : function(std::forward<F>(callback)) {}

		Callback function;
		ASTNodePtr node;
		ASTNode* nodeRef {};
		bool passThrough = false;
	};
	using NativeFunctionPtr = std::shared_ptr<NativeFunction>;
}
