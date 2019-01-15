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

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunction() {
			return node;
		}

		inline auto& GetFunctionPrototype() {
			return node[0];
		}

		inline auto& GetFunctionBody() {
			return node[1];
		}
	};
}
