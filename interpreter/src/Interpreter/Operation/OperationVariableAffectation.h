#pragma once

#include "Operation.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::VARIABLE_AFFECTATION> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}
		inline auto& GetVariableNameNode() {
			return node[0];
		}
		
		inline auto& GetVariableValueNode() {
			return node[1];
		}
	};
}
