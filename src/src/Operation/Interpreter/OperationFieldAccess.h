#pragma once
#include <cassert>
#include "Operation.h"
#include "Service/Script.h"

namespace ska {
	class ASTNode;
	template<>
    class Operation<Operator::FIELD_ACCESS> {
	private:
		ASTNode& node;
				
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetObject() {
			return node[0];
		}
		
		inline std::string GetFieldName() {
			return node[1].name();
		}
				
	};
}
