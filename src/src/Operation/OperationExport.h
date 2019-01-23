#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::EXPORT> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetVariable() {
			assert(node.size() == 1);
			return node[0];
		}
		

	};
}
