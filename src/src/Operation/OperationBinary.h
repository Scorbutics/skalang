#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BINARY> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetFirstValue() {
			return node[0];
		}
		
		inline auto& GetSecondValue() {
			return node[1];
		}
	};
}
