#pragma once

#include "NodeValue/AST.h"
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BRIDGE> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetObject() {
			return node[0];
		}
	};
}
