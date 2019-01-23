#pragma once

#include "OperationType.h"

namespace ska {
	class ASTNode;
	template<>
	class OperationType<Operator::IF_ELSE> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}
	};

	template<>
	class OperationType<Operator::IF> {
	private:
		ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}
	};
}