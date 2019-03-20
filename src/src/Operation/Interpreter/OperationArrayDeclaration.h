#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::ARRAY_DECLARATION> {
	private:
		ASTNode& node;
		
	public:
		Script& parent;

		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto GetArrayName() {
			return node[0].name();
		}

        inline auto GetArraySubType() {
            return node[0].type().value();
        }

		inline auto begin() {
			return node.begin();
		}
		
		inline auto end() {
			return node.end();
		}
		
		inline const auto begin() const {
			return node.begin();
		}
		
		inline const auto end() const {
			return node.end();
		}
	};
}
