#pragma once

#include "Operation.h"
#include "Service/Script.h"

namespace ska {
	class ASTNode;
	template<>
	class Operation<Operator::IF_ELSE> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetCondition() {
			return node[0];
		}

		inline auto& GetIfStatement() {
			return node[1];
		}

		inline auto& GetElseStatement() {
			return node[2];
		}
	};

	template<>
	class Operation<Operator::IF> {
	private:
		ASTNode& node;
	public:
		Script parent;
		Operation(ExecutionContext& context) : node(context.pointer()), parent(context.program()) {}

		inline auto& GetCondition() {
			return node[0];
		}

		inline auto& GetIfStatement() {
			return node[1];
		}		
	};
}