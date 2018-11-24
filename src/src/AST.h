#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "OperatorTraits.h"
#include "Operator.h"
#include "ExpressionType.h"
#include "TypeBuildUnit.h"

//Alls except Unary, Binary & Literal
#include "TypeBuilderFieldAccess.h"
#include "TypeBuilderFunctionCall.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderParameterDeclaration.h"
#include "TypeBuilderVariableAffectation.h"
#include "TypeBuilderVariableDeclaration.h"

namespace ska {
	class Symbol;
    class SymbolTable;

	class ASTNode {
	public:
		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;

		bool has(const Token& t) const {
			return token == t;
		}

		bool logicalEmpty() const {
			return token.type() == TokenType::EMPTY && m_op == Operator::UNARY;
		}

		std::string name() const {
			return token.name();
		}

		std::size_t size() const {
			return m_children.size();
		}

		TokenType tokenType() const {
			return token.type();
		}

		ASTNode& left() {
			assert(m_op == Operator::BINARY && !m_children.empty());
			return *m_children[0].get();
		}

		ASTNode& right() {
			assert(m_op == Operator::BINARY && m_children.size() >= 2);
			return *m_children[1].get();
		}

		const ASTNode& left() const {
			assert(m_op == Operator::BINARY && !m_children.empty());
			return *m_children[0].get();
		}

		const ASTNode& right() const {
			assert(m_op == Operator::BINARY && m_children.size() >= 2);
			return *m_children[1].get();
		}

		auto& operator[](const std::size_t index) {
		    return *m_children[index];
		}

		const auto& operator[](const std::size_t index) const {
		    return *m_children[index];
		}

		auto begin() { return std::begin(m_children); }
		auto end() { return std::end(m_children); }

		const auto begin() const { return std::begin(m_children); }
		const auto end() const { return std::end(m_children); }

		const auto& op() const {
			return m_op;
		}

	    void buildType(const SymbolTable& symbols);

		const auto& type() const {
			return m_type;
		}

        template<Operator o, class ... Node>
        static ASTNodePtr MakeNode(std::unique_ptr<Node>&& ... children) {
			return ASTNode::template MakeNode<o>(Token{}, ASTNode::template BuildVectorFromNodePack(std::move(children)...));
        }

        template<Operator o, class ... Node>
		static ASTNodePtr MakeNode(Token token, std::unique_ptr<Node>&& ... children) {
            return ASTNode::template MakeNode<o>(std::move(token), ASTNode::template BuildVectorFromNodePack(std::move(children)...));
        }

		template<Operator o>
		static ASTNodePtr MakeNode(std::vector<ASTNodePtr> children) {
			return ASTNode::template MakeNode<o>(Token {}, std::move(children));
		}

        template <Operator o>
		static ASTNodePtr MakeNode(Token token, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{}) {
			ASTNode::template CheckTokenAssociatedWithOperator<o>(token);
            auto node = std::unique_ptr<ASTNode>(new ASTNode(o, std::move(token), std::move(children)));
			node->m_typeBuilder = std::make_unique<TypeBuilderOperator<o>>();
			return node;
        }

        static ASTNodePtr MakeEmptyNode() {
            return std::unique_ptr<ASTNode>(new ASTNode());
        }

        static ASTNodePtr MakeLogicalNode(Token token, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr) {
			assert(!token.empty());
            return std::unique_ptr<ASTNode>(new ASTNode(std::move(token), std::move(l), std::move(r)));
        }

	private:
		ASTNode() = default;
        
		template<Operator o>
		static void CheckTokenAssociatedWithOperator(const Token& token) {
			static_assert(o != Operator::BINARY && o != Operator::LITERAL, "Wrong constructor used for a logical ASTNode. Use MakeLogicalNode instead.");
			if constexpr (OperatorTraits::isNamed(o)) {
				assert(!token.empty());
			} else {
				assert(token.empty());
			}
		}

        explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr);
        ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{});
        ASTNode(Operator o, Token identifierToken = Token{});

		template <class Func, class ... Args>
		static void for_each(Func&& f, Args&& ... t) {
			(f(std::forward<Args>(t)), ...);
		}

		template <class ... Node>
		static std::vector<ASTNodePtr> BuildVectorFromNodePack(std::unique_ptr<Node>&& ... children) {
			auto result = std::vector<ASTNodePtr>{};
			for_each([&result](auto&& n) {
				result.push_back(std::forward<decltype(n)>(n));
			}, std::forward<std::unique_ptr<Node>>(children)...);
			return std::move(result);
		}

		Operator m_op = Operator::UNARY;
		std::optional<Type> m_type;
		std::unique_ptr<TypeBuildUnit> m_typeBuilder;

		Token token;
		std::vector<ASTNodePtr> m_children;

		friend std::ostream& operator<<(std::ostream& stream, const ASTNode& node);
	};

	inline std::ostream& operator<<(std::ostream& stream, const ASTNode& node) {
		stream << node.token;
		return stream;
	}
}

