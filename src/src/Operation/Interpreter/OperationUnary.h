#pragma once

#include "Operation.h"
#include "NodeValue/AST.h"
#include "Service/Script.h"

namespace ska {
	template<>
	class Operation<Operator::UNARY> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

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
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

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
