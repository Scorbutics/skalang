#pragma once

#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::ARRAY_USE> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetArrayName() {
			return node[0].name();
		}

		inline auto& GetArrayIndexNode() {
			return node[1];
		}

        inline auto GetArraySubType() {
            assert(node.size() == 2 && node[0].type().has_value() && node[0].type().value().compound().size() == 1);
            return node[0].type().value().compound()[0];
        }
	};
}
