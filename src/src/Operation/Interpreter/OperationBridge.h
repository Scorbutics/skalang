#pragma once

#include "NodeValue/AST.h"
#include "Operation.h"
#include "Service/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BRIDGE> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetObject() {
			return node[0];
		}
	};
}
