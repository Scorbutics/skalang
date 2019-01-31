#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::ARRAY_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}
		
		inline auto HasArrayType() {
			return node.size() > 0;
		}

        inline auto GetArraySubType() {
			assert(HasArrayType());
            return node[0].type().value();
        }
		
		auto begin() const {
			return node.begin();
		}

		auto end() const  {
			return node.end();
		}
	};
}
