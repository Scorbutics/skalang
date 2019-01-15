#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::ARRAY_USE> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetArrayName() {
			return node[0].name();
		}

		inline auto& GetArrayIndexNode() {
			return node[1];
		}
	};
}
