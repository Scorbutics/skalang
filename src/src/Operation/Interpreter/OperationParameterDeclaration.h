#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::PARAMETER_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetVariableName() {
			assert(!node.name().empty());
			return node.name();
		}
		
		inline auto& GetVariableValueNode() {
			assert(node.size() == 1);
			return node[0];
		}
	};
}
