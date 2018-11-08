#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "Operator.h"
#include "ExpressionType.h"

namespace ska {
	class Symbol;

	class ASTNode {
	public:
		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		bool has(const Token& t) const {
			return token == t;
		}

		bool logicalEmpty() const {
			return token.type() == TokenType::EMPTY && !m_op.has_value();
		}

		std::string asString() const {
			const auto stringRepresentation = token.asString();
			return stringRepresentation.empty() && !children.empty() ? "¤" : stringRepresentation;
		}

		std::size_t size() const {
			return children.size();
		}

        /*
		template<class T>
		void add(std::unique_ptr<T> c) {
			assert(c != nullptr);
			assert(m_op != Operator::UNARY && m_op != Operator::BINARY);
            children.emplace_back(std::move(c));
		}

		template<class T>
		void addIfExists(std::unique_ptr<T> c) {
			assert(m_op != Operator::UNARY && m_op != Operator::BINARY);
            if(c != nullptr) {
				children.emplace_back(std::move(c));
		    }
		}
        */

		TokenType tokenType() const {
			return token.type();
		}

		ASTNode& left() {
			assert(m_op == Operator::BINARY && !children.empty());
			return *children[0].get();
		}

		ASTNode& right() {
			assert(m_op == Operator::BINARY && children.size() >= 2);
			return *children[1].get();
		}

		const ASTNode& left() const {
			assert(m_op == Operator::BINARY && !children.empty());
			return *children[0].get();
		}

		const ASTNode& right() const {
			assert(m_op == Operator::BINARY && children.size() >= 2);
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

		const auto begin() const { return std::begin(children); }
		const auto end() const { return std::end(children); }

		const auto& op() const {
			return m_op;
		}

		auto& type() {
			return m_type;
		}

		const auto& type() const {
			return m_type;
		}

        template<Operator o, class NodePtr>
        static ASTNodePtr MakeNode(NodePtr&& ... children) {
            static_assert(o != Operator::BINARY && o != Operator::LITERAL, "Wrong constructor used for a logical ASTNode. Use MakeLogicalNode instead.");
            return std::unique_ptr<ASTNode>(new ASTNode(o, std::move(token), std::move(children) ...));
        }

        template<Operator o, class NodePtr>
        static ASTNodePtr MakeNode(Token token, NodePtr&& ... children) {
            static_assert(o != Operator::BINARY && o != Operator::LITERAL, "Wrong constructor used for a logical ASTNode. Use MakeLogicalNode instead.");
            if constexpr(
                    o == Operator::VARIABLE_DECLARATION || 
                    o == Operator::FUNCTION_DECLARATION || 
                    o == Operator::PARAMETER_DECLARATION) {
                assert(!token.empty());
            } else {
                assert(token.empty());
            }

            return std::unique_ptr<ASTNode>(new ASTNode(o, std::move(token), std::move(children) ...));
        }

        static ASTNodePtr MakeEmptyNode() {
            return std::unique_ptr<ASTNode>(new ASTNode());
        }

        static ASTNodePtr MakeLogicalNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr) {
            return std::unique_ptr<ASTNode>(new ASTNode(std::move(t), std::move(l), std::move(r)));
        }

	private:
		ASTNode() = default;
        
        explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr) :
			m_op(l != nullptr && r != nullptr ? Operator::BINARY : Operator::UNARY),
			token(std::move(t)) {
			if (l != nullptr) {
				add(std::move(l));
			}

			if (r != nullptr) {
				add(std::move(r));
			}

            if(token.isLiteral()) {
                assert(m_op == Operator::UNARY);
                m_op = Operator::LITERAL;
            }
            
		}

        ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{}) : 
            m_op(o) {
            if(!children.empty()) {
                m_children.reserve(children.size());
                for(auto& child : children) {
                    if(child != nullptr) {
                        m_children.push_back(std::move(child));
                    }
                }
            }
        }

        ASTNode(Operator o, Token identifierToken = Token{}) :
			m_op(std::move(o)),
			token(std::move(identifierToken)) {
        }

		Operator m_op;
		std::optional<Type> m_type;

		Token token;
		std::vector<ASTNodePtr> m_children;

	};
}

