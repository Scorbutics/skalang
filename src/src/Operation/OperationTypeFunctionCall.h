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
		
		const Type GetFunctionReturnSymbol(const SymbolTable& symbols);
	};
}
