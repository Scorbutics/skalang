#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::FOR_LOOP> {
	private:
		ASTNode& node;
	public:
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetCondition() {
			return node[1];
		}

		inline auto& GetIncrement() {
			return node[2];
		}
		
		inline auto& GetInitialization() {
			return node[0];
		}
		
		inline auto& GetStatement() {
			return node[3];
		}
	};

}