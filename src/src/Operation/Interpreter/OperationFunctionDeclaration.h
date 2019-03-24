#pragma once

#include "Operation.h"
#include "Service/Script.h"

namespace ska {
	class ASTNode;
	
	template<>
	class Operation<Operator::FUNCTION_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Script parent;
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
	
}
