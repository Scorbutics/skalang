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
		
		inline auto& GetObject() {
			return node[0];
		}
		
		inline std::string GetFieldName() {
			return node[1].name();
		}
				
	};
}
