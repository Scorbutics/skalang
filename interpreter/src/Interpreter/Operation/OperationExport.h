#pragma once
#include <cassert>
#include "Operation.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::EXPORT> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetVariable() {
			assert(node.size() == 1);
			return node[0];
		}
		

	};
}
