#pragma once

#include "Operation.h"

namespace ska {
	class ASTNode;
	
	template<>
	class Operation<Operator::FUNCTION_CALL> {
	private:
		ASTNode& node;
	public:
		Operation(ASTNode& node) : node(node) {}

		inline auto& GetFunction() {
			return node[0];
		}
		
		auto begin() {
			return node.begin() + 1;
		}

		auto end() {
			return node.end();
		}

		inline auto& GetFunctionParameterValue(std::size_t index) {
			return node[index + 1];
		}

		inline bool HasFunctionParameter(std::size_t index) const {
			return (node.size() - 1) > index;
		}
	};
}
