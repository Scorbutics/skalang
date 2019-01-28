#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::RETURN> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetValue() {
			return node[0];
		}
	};

}