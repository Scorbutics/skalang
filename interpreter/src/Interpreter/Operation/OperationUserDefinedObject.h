#pragma once
#include <cassert>
#include "Operation.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::USER_DEFINED_OBJECT> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		auto begin() {
			return node.begin();
		}
		
		auto end() {
			return node.end();
		}
	};
}
