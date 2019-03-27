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

		inline auto GetScriptPath() {
			return node[0].name();
		}
	};
}
