#pragma once
#include <cassert>
#include "Operation.h"
#include "Interpreter/ExecutionContext.h"

namespace ska {
	class ASTNode;
	class Script;

	template<>
	class Operation<Operator::PARAMETER_DECLARATION> {
	private:
		ASTNode& node;
	public:
		Script& parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

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
