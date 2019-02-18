#pragma once
#include <cassert>

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
    class OperationType<Operator::IMPORT> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

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

		inline auto GetScriptPath() {
			return node[5].name();
		}
	};
}
