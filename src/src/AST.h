#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "Operator.h"
#include "ExpressionType.h"

namespace ska {

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
				op = token.empty() ? std::optional<Operator>() : Operator::LITERAL;
			}
		}

		ASTNode(Operator o, Token identifierToken = Token{}) :
			op(std::move(o)),
			token(std::move(identifierToken)) {
		}

		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		bool has(const Token& t) const {
			return token == t;
		}

		bool empty() const {
			return token.type() == TokenType::EMPTY && !op.has_value();
		}

		std::string asString() const {
			const auto stringRepresentation = token.asString();
			return stringRepresentation.empty() && !children.empty() ? "¤" : stringRepresentation;
		}

		std::size_t size() const {
			return children.size();
		}

		template<class T>
		void add(std::unique_ptr<T> c) {
			assert(c != nullptr);
			children.emplace_back(std::move(c));
		}

		template<class T>
		void addIfExists(std::unique_ptr<T> c) {
		    if(c != nullptr) {
				children.emplace_back(std::move(c));
		    }
		}

		TokenType tokenType() const {
			return token.type();
		}

		ASTNode& left() {
			assert(op == Operator::BINARY && !children.empty());
			return *children[0].get();
		}

		ASTNode& right() {
			assert(op == Operator::BINARY && children.size() >= 2);
			return *children[1].get();
		}

		auto& operator[](const std::size_t index) {
		    return *children[index];
		}

		const auto& operator[](const std::size_t index) const {
		    return *children[index];
		}

		auto begin() { return std::begin(children); }
		auto end() { return std::end(children); }

		auto begin() const { return std::begin(children); }
		auto end() const { return std::end(children); }

		std::optional<Operator> op;
		std::optional<Type> type;

	private:
		Token token;
		std::vector<std::unique_ptr<ASTNode>> children;

	};

}
