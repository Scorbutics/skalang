#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	
	template<>
    class OperationType<Operator::PARAMETER_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetVariableName() {
			assert(!node.name().empty());
			return node.name();
		}
		
		inline const auto& GetTypeValueNode() {
			assert(node.size() == 1);
			return node[0];
		}

	};
}
