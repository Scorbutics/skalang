#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::UNARY> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetValue() {
			return node.name();
		}
	};
	
	template<>
	class Operation<Operator::LITERAL> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetValue() {
			return node.name();
		}
	};
}
