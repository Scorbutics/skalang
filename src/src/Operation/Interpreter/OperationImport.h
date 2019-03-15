#pragma once
#include <cassert>

#include "Operation.h"
#include "Interpreter/ExecutionContext.h"

namespace ska {
	class ASTNode;

	template<>
    class Operation<Operator::IMPORT> {
	private:
		ASTNode& node;
	public:
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetVariableNode() {
			return node[0];
		}

		inline auto& GetValueNode() {
			return node[1];
		}
		
		ASTNode* GetScriptNode();

		/*
		inline auto& GetScriptNode() {
			return node[2];
		}
		*/

	};
}
