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
	class Script;

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

		TokenType tokenType() const {
			return token.type();
		}

		Token::Variant tokenContent() const {
			return token.content();
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

		const auto& type() const {
			return m_type;
		}

		auto& script() { assert(m_linkedScript != nullptr);  return *m_linkedScript; }
		const auto& script() const { assert(m_linkedScript != nullptr); return *m_linkedScript;	}

	private:
		friend class ASTFactory;
		ASTNode();
        
        explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr);
		explicit ASTNode(Script s, std::vector<ASTNodePtr> children);

        ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{});
        ASTNode(Operator o, Token identifierToken = Token{});


		Operator m_op = Operator::UNARY;
		std::optional<Type> m_type;

		//TODO a externaliser
		std::unique_ptr<TypeBuildUnit> m_typeBuilder;

		Token token;
		std::vector<ASTNodePtr> m_children;

		struct ScriptPtr : std::unique_ptr<Script> {
			using std::unique_ptr<Script>::unique_ptr;
			~ScriptPtr(); // Implement (empty body) elsewhere
		} m_linkedScript;

		friend std::ostream& operator<<(std::ostream& stream, const ASTNode& node);
	};

	inline std::ostream& operator<<(std::ostream& stream, const ASTNode& node) {
		stream << node.token;
		return stream;
	}
}

