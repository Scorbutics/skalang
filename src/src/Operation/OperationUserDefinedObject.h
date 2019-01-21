#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::USER_DEFINED_OBJECT> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto begin() {
			return node.begin();
		}
		
		auto end() {
			return node.end();
		}
	};
}
