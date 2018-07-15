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

		ASTNode(Token t, std::unique_ptr<ASTNode> l = nullptr, std::unique_ptr<ASTNode> r = nullptr) :
			token(std::move(t)),
			left(std::move(l)),
			right(std::move(r)) {
		}

		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		bool empty() const {
			return token.type() == TokenType::EMPTY;
		}

		Operator getOperator() const {
			return m_operator;
		}

		std::string asString() const {
			if(left != nullptr || right != nullptr) {
				return "¤";
			}
			return token.asString();
		}

		Token token;
		std::unique_ptr<ASTNode> left;
		std::unique_ptr<ASTNode> right;
		ASTNode* parent = this;
		std::vector<std::unique_ptr<ASTNode>> otherChildren;
	private:
		Operator m_operator;
		
	};
}