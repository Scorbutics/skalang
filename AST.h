#pragma once
#include <vector>
#include <memory>
#include "Token.h"

namespace ska {
	class ASTNode {
	public:
		ASTNode() = default;

		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		ASTNode& addChild(Token token) {
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

		Token token;
	private:
		ASTNode* m_parent = this;
		std::vector<std::unique_ptr<ASTNode>> m_children;
	};
}