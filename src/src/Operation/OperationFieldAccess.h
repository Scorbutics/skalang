#pragma once
#include <cassert>
#include "Operation.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::FIELD_ACCESS> {
	private:
		ASTNode& node;
		
		inline auto& GetObject() {
			return node[0];
		}
		
	public:
		Operation(ASTNode& node) : node(node) {}
		
		inline std::string GetObjectName() {
			return node[0].name();
		}
		
		inline std::string GetFieldName() {
			return node[1].name();
		}
				
	};
}
