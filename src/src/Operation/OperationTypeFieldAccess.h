#pragma once
#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	class SymbolTable;
	class Symbol;
	
	template<>
    class OperationType<Operator::FIELD_ACCESS> {
	private:
		const ASTNode& node;
		
		inline auto& GetObject() {
			return node[0];
		}
		
	public:
		OperationType(const ASTNode& node) : node(node) {}
		
		inline std::string GetObjectName() {
			return node[0].name();
		}
		
		inline std::string GetFieldName() {
			return node[1].name();
		}
		
		inline std::string GetObjectTypeName() {
			const auto objectType = GetObject().type().value();
			return objectType.getName();
		}
		
		Type GetObjectType(const SymbolTable& symbols);
		
		Symbol* GetFieldSymbolFromObjectSymbolTable(const Type& objectType);
		
	};
}
