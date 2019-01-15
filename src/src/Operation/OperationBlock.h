#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::BLOCK> {
	public:
		Operation(ASTNode& node) {}
	};
}
