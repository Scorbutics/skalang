#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::IF_ELSE> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto& GetCondition() {
			return node[0];
		}

		auto& GetIfStatement() {
			return node[1];
		}

		auto& GetElseStatement() {
			return node[2];
		}
	};

	template<>
	class Operation<Operator::IF> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		auto& GetCondition() {
			return node[0];
		}

		auto& GetIfStatement() {
			return node[1];
		}		
	};
}