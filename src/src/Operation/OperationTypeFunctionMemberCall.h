#pragma once

#include <cassert>
#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::FUNCTION_MEMBER_CALL> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) { assert(node.size() >= 2); }

		inline auto GetFunctionType() {
			return node[0].type().value();
		}

		inline const auto& GetFunctionNameNode() {
			return node[0];
		}

		auto begin() const {
			return node.begin() + 2;
		}

		auto end() const {
			return node.end();
		}

		auto rbegin() const {
			return node.rbegin();
		}

		auto rend() const {
			return node.rend() - 2;
		}

		inline std::size_t GetFunctionParameterSize() const {
			return node.size() - 2;
		}

	};
}
