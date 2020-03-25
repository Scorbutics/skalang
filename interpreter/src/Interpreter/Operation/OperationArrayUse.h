#pragma once

#include <cassert>
#include "Operation.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::ARRAY_USE> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetArrayNode() {
			return node[0];
		}

		inline auto& GetArrayIndexNode() {
			return node[1];
		}

    	inline auto GetArraySubType() {
        	assert(node.size() == 2 && node[0].type().has_value() && node[0].type().value().size() == 1);
        	return node[0].type().value()[0];
    	}
	};
}
