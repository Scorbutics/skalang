#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class ASTNode;
	
	template<>
	class OperationType<Operator::TYPE> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) { assert(node.size() == 3); }		

		const Symbol* GetTypeSymbol(const SymbolTable& symbolTable) const;

		bool IsBuiltIn() const;
		bool IsObject() const;
		std::string GetName() const;
		std::string GetTypeName() const;

		bool IsArray() const  {
			return node.size() > 2 && !node[2].logicalEmpty();
		}
	};
}
