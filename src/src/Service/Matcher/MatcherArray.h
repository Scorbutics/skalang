#pragma once
#include <memory>
#include <string>

#include "Container/expression_stack.h"
#include "NodeValue/Token.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;

	class MatcherArray {
		using ExpressionStack = expression_stack<Token, ASTNodePtr>;
	public:
		~MatcherArray() = default;
		MatcherArray(TokenReader& input, const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_input(input), m_reservedKeywordsPool(pool), m_parser(parser) {}
		
		[[nodiscard]]
		ASTNodePtr match(ExpressionStack& operands, char token, bool isDoingOperation);
	
	private:
		ASTNodePtr matchUse(ASTNodePtr identifierArrayAffected);
		ASTNodePtr matchDeclaration();

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}