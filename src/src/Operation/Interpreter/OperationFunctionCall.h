#pragma once

#include "Operation.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class ASTNode;
	
	template<>
	class Operation<Operator::FUNCTION_CALL> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

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
