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

		auto& GetVariableNameNode() {
			return node[0];
		}
		
		auto& GetVariableValueNode() {
			return node[1];
		}
	};
}
