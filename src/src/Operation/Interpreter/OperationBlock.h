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
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

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
