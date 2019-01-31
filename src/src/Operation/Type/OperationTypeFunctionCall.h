#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class Symbol;
	
	template<>
	class OperationType<Operator::FUNCTION_CALL> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionType() {
			return node[0].type().value();
		}

		auto begin() {
			return node.begin() + 1;
		}
		
		auto end() {
			return node.end();
		}

		inline std::size_t GetFunctionParameterSize() const {
			return node.size() - 1;
		}

	};
}
