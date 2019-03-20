#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	
	template<>
	class Operation<Operator::FUNCTION_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunction() {
			return node;
		}

		inline auto& GetFunctionPrototype() {
			return node[0];
		}

		inline auto& GetFunctionBody() {
			return node[1];
		}
	};
	
	template<>
	class Operation<Operator::FUNCTION_PROTOTYPE_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Script& parent;

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunction() {
			return node;
		}

		auto begin() {
			return node.begin();
		}
		
		auto end() {
			return node.end();
		}

	};
}
