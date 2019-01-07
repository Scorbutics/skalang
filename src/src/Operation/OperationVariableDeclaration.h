#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::VARIABLE_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto GetVariableName() {
			assert(!node.name().empty());
			return node.name();
		}
		
		auto& GetVariableValueNode() {
			return node[0];
		}
	};
}
