#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Token.h"

namespace ska {

	enum class Operator {
		BINARY,
		UNARY,
		LITERAL,
		VARIABLE_DECLARATION,
		FUNCTION_CALL,
		FUNCTION_DECLARATION,
		FOR_LOOP,
		BLOCK,
		IF,
		IF_ELSE
	};

	class ASTNode {
	public:
		ASTNode() = default;

		ASTNode(Token t, std::unique_ptr<ASTNode> l = nullptr, std::unique_ptr<ASTNode> r = nullptr) :
			op(Operator::BINARY),
			token(std::move(t)) {
			if (l != nullptr) {
				add(std::move(l));
			}

			if (r != nullptr) {
				add(std::move(r));
			}

			if (r == nullptr && l == nullptr) {
				op = Operator::LITERAL;
			}
		}

		ASTNode(Operator o, Token identifierToken = Token{}) :
			op(std::move(o)),
			token(std::move(identifierToken)) {
		}

		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		bool empty() const {
			return token.type() == TokenType::EMPTY && !op.has_value();
		}

		std::string asString() const {
			if(!children.empty()) {
				//Compound ASTNode
				return "¤";
			}
			return token.asString();
		}

		template<class T>
		void add(std::unique_ptr<T> c) {
			assert(c != nullptr);
			children.emplace_back(std::move(c));
		}

		ASTNode& left() {
			assert(op == Operator::BINARY && !children.empty());
			return *children[0].get();
		}

		ASTNode& right() {
			assert(op == Operator::BINARY && children.size() >= 2);
			return *children[1].get();
		}

		auto begin() { return std::begin(children); }
		auto end() { return std::end(children); }

		auto begin() const { return std::begin(children); }
		auto end() const { return std::end(children); }

		std::optional<Operator> op;
		Token token;		
		//ASTNode* parent = this;

	private:
		std::vector<std::unique_ptr<ASTNode>> children;

	};
}