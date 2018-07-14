#pragma once
#include <vector>
#include <memory>
#include "Token.h"

namespace ska {

	enum class Operator {
		BINARY,
		UNARY,
		NUMERIC
	};

	class ASTNode {
	public:
		ASTNode() = default;

		ASTNode(Token t) :
			token(std::move(t)) {
		}

		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		ASTNode& addOtherChild(Token token) {
			m_children.emplace_back(std::make_unique<ASTNode>());
			auto& last = *m_children.back().get();
			last.token = std::move(token);
			last.m_parent = this;
			return last;
		}

		std::vector<std::unique_ptr<ASTNode>>& children() {
			return m_children;
		}

		bool empty() const {
			return token.type == TokenType::EMPTY;
		}

		ASTNode& parent() {
			return *m_parent;
		}

		Operator getOperator() const {
			return m_operator;
		}

		Token token;
		std::unique_ptr<ASTNode> left;
		std::unique_ptr<ASTNode> right;
	private:
		Operator m_operator;
		ASTNode* m_parent = this;
		std::vector<std::unique_ptr<ASTNode>> m_children;
	};
}