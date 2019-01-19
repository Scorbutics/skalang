#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::FIELD_ACCESS> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		std::string GetObject() {
			return node[0].name();
		}
		
		std::string GetField() {
			return node[1].name();
		}
	};
}
