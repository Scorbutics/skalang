#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::FUNCTION_CALL> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetFunctionName() {
			return node[0].name();
		}

		auto begin() {
			return node.begin() + 1;
		}

		auto end() {
			return node.end();
		}

	};
}
