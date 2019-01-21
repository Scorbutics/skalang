#pragma once
#include <vector>
#include <variant>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "Operator.h"
#include "ExpressionType.h"
#include "Interpreter/MemoryTable.h"
#include "Interpreter/NodeValue.h"

#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Type.h"

namespace ska {
	class Symbol;
    class SymbolTable;
	class StatementParser;
	class ASTFactory;

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

		Token::Variant tokenContent() const {
			return token.content();
		}

		const auto& value() const {
			return m_value;
		}

		std::string valueAsString() const {
			return "";
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

		ASTNodePtr stealChild(const std::size_t index) {
			return std::move(m_children[index]);
		}

		auto begin() { return std::begin(m_children); }
		auto end() { return std::end(m_children); }

		const auto begin() const { return std::begin(m_children); }
		const auto end() const { return std::end(m_children); }

		const auto& op() const {
			return m_op;
		}

	    void buildType(const SymbolTable& symbols);
		void buildValue(NodeValue value);

		const auto& type() const {
			return m_type;
		}

	private:
		friend class ASTFactory;
		ASTNode() = default;
        
        explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr);
        ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{});
        ASTNode(Operator o, Token identifierToken = Token{});

		Operator m_op = Operator::UNARY;
		std::optional<Type> m_type;
		std::unique_ptr<TypeBuildUnit> m_typeBuilder;

		Token token;
		NodeValue m_value;
		std::vector<ASTNodePtr> m_children;

		friend std::ostream& operator<<(std::ostream& stream, const ASTNode& node);
	};

	inline std::ostream& operator<<(std::ostream& stream, const ASTNode& node) {
		stream << node.token;
		return stream;
	}
}

