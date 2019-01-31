#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::BINARY> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}
		
		inline auto GetFirstType() {
			return node[0].type().value();
		}
	
		inline auto GetSecondType() {
			return node[1].type().value();
		}

		inline auto GetOperator() {
			return node.name();
		}
	};
}
