#pragma once

#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::FACTORY_PROTOTYPE_DECLARATION> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {}

		inline auto GetFunctionName() {
			return node.name();
		}

		inline auto& GetFunctionParametersList() {
			return node[0];
		}

		inline auto GetFunctionParametersSize() {
			return node[0].size() - 1;
		}

		inline const auto* GetPrivateFunctionFactoryField(const std::string& fieldName) {
			return (*node[1].symbol())(fieldName);
		}

		inline auto& GetPrivateFunctionFactoryNode() {
			return node[1];
		}

		inline auto GetFunctionReturnType() {
			return (*node[0].rbegin())->type().value();
		}

		inline auto rbegin() { return node[0].rbegin(); }
		inline auto rend() { return node[0].rend(); }

	};
}
