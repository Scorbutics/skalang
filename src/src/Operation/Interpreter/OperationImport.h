#pragma once
#include <cassert>

#include "Operation.h"
#include "Interpreter/Value/ExecutionContext.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;

	template<>
    class Operation<Operator::IMPORT> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		ScriptPtr GetScript();
	};
}
