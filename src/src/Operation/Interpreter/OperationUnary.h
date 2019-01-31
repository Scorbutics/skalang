#pragma once

#include "Operation.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class Operation<Operator::UNARY> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetValue() {
			return node.name();
		}
		
		inline auto& asNode() {
			return node;
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

		inline const auto& GetType() const {
			return node.type();
		}
		
		inline auto& asNode() {
			return node;
		}
	};
}
