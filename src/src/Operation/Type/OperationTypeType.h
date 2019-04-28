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

		const Symbol* GetSymbol(const SymbolTable& symbolTable) const ;
		bool IsBuiltIn() const;
		bool IsObject() const;
		std::string GetName() const;

		bool IsArray() const  {
			return !node[2].logicalEmpty();
		}
	};
}
