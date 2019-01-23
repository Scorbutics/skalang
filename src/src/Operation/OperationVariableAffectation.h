#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::VARIABLE_AFFECTATION> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetVariableNameNode() {
			return node[0];
		}
		
		inline auto& GetVariableValueNode() {
			return node[1];
		}
	};
}
