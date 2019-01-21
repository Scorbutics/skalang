#pragma once
#include <cassert>
#include "OperationType.h"

namespace ska {
	class ASTNode;
	template<>
    class OperationType<Operator::USER_DEFINED_OBJECT> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		auto begin() {
			return node.begin();
		}
		
		auto end() {
			return node.end();
		}
	};
}
