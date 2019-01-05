#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::VARIABLE_AFFECTATION> {
	protected:
		static auto& GetVariableNameNode(ASTNode& node) {
			return node[0];
		}
		
		static auto& GetVariableValueNode(ASTNode& node) {
			return node[1];
		}
	};
}
