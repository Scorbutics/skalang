#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::FUNCTION_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto GetFunctionName() {
			return node.name();
		}

		auto& GetFunction() {
			return node[0];
		}
	};
}
