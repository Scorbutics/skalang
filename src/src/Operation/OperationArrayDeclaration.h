#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::ARRAY_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto GetArrayName() {
			return node[0].name();
		}

		auto& begin() {
			return node.begin();
		}
		
		auto& end() {
			return node.end();
		}
	};
}
