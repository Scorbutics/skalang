#pragma once

#include "Operation.h"
#include "Service/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::RETURN> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetValue() {
			return node[0];
		}
	};

}