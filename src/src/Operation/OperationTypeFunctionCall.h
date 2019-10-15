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

		inline const auto& GetFunctionNameNode() {
			return node[0];
		}

		auto begin() const {
			return node.begin() + 1;
		}

		auto end() const {
			return node.end();
		}

		inline std::size_t GetFunctionParameterSize() const {
			return node.size() - 1;
		}

	};
}
