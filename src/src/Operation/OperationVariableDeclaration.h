#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::VARIABLE_DECLARATION> {
	protected:
		static auto GetVariableName(ASTNode& node) {
			assert(!node.name().empty());
			return node.name();
		}
		
		static auto& GetVariableValueNode(ASTNode& node) {
			return node[0];
		}
	};
}
