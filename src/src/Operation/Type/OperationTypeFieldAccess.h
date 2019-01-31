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
		
		inline auto& GetField() {
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
		
        inline auto GetObjectType() {
            return GetObject().type().value();
        }

		inline auto GetFieldType() {
			const auto objectType = GetField().type().value();
			return objectType;
		}
	};
}
