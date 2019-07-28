#pragma once
#include <vector>
#include <variant>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "Operator.h"
#include "ExpressionType.h"
#include "Interpreter/Value/ScriptPtr.h"
#include "Interpreter/MemoryTable.h"
#include "Interpreter/Value/NodeValue.h"

#include "Service/TypeBuilder/TypeBuildersContainer.h"
#include "Type.h"

namespace ska {
	struct Cursor;
	class Symbol;
	class StatementParser;
	class ASTFactory;

	class ASTNode {
	public:
		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;
		~ASTNode() = default;

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

		const Cursor& positionInScript() const {
			return token.position();
		}

		TokenType tokenType() const {
			return token.type();
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

	    void buildType(const TypeBuildersContainer& typeBuilder, const Script& script);
		void linkSymbol(const Symbol& symbol) {
			m_symbol = &symbol;
		}

		const auto& type() const {
			return m_type;
		}

		const auto* symbol() const {
			return m_symbol;
		}

	private:
		friend class ASTFactory;
		ASTNode();
        
        explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr);

        ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{});
        ASTNode(Operator o, Token identifierToken = Token{});

		Operator m_op = Operator::UNARY;
		std::optional<Type> m_type;
		const Symbol* m_symbol = nullptr;

		Token token;
		std::vector<ASTNodePtr> m_children;

		friend std::ostream& operator<<(std::ostream& stream, const ASTNode& node);
	};

	inline std::ostream& operator<<(std::ostream& stream, const ASTNode& node) {
		stream << node.token;
		return stream;
	}
}

