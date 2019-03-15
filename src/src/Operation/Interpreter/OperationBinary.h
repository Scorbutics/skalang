#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BINARY> {
	private:
		ASTNode& node;
	public:
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetFirstValue() {
			return node[0];
		}
		
		inline auto& GetSecondValue() {
			return node[1];
		}
		
		inline auto& GetType() {
			return node.type();
		}
	
		inline auto GetOperator() {
			return node.name();
		}
	};
}
