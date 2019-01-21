#pragma once
#include <cassert>

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::IMPORT> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto GetImport() {
			assert(!node.name().empty());
			return node.name();
		}
		
		inline auto& GetVariableValueNode() {
			return node[0];
		}
		
		inline auto GetImportVariableName() {
			return node[0].name();
		}
	};
}
