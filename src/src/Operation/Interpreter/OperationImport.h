#pragma once
#include <cassert>

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::IMPORT> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}
	
		inline auto& GetVariableNode() {
			return node[0];
		}

		inline auto& GetValueNode() {
			return node[1];
		}
		
		inline auto& GetScriptNode() {
			return node[2];
		}

	};
}
