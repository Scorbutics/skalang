#pragma once

#include "NodeValue/AST.h"
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BLOCK> {
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

		auto size() {
			return node.size();
		}
	};
}
