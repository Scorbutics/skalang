#pragma once

#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::ARRAY_USE> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetArrayName() {
			return node[0].name();
		}

        inline auto GetArraySubType() {
            assert(node.size() == 2 && node[0].type().has_value() && node[0].type().value().compound().size() == 1);
            return node[0].type().value().compound()[0];
        }
	};
}
