#pragma once
#include <vector>
#include "Token.h"
#include "Scope.h"
#include "ShuntingYardExpressionParser.h"

namespace ska {
	struct ReservedKeywordsPool;

	class Parser {
		friend class ShuntingYardExpressionParser;
		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
	public:
		Parser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input);
		std::pair<ASTNodePtr, Scope> parse();

	private:
		ASTNodePtr statement();
        ASTNodePtr optstatement(const Token& mustNotBe = Token{});

		ASTNodePtr matchReservedKeyword(const std::size_t keywordIndex);

        ASTNodePtr matchBlock(const std::string& content);
        ASTNodePtr matchForKeyword();
        ASTNodePtr matchIfOrIfElseKeyword();
        ASTNodePtr matchVarKeyword();
        ASTNodePtr matchExpressionStatement();

		ASTNodePtr expr();
		ASTNodePtr optexpr(const Token& mustNotBe = Token{});

		static void error();

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ShuntingYardExpressionParser m_shuntingYardParser;
	};
}
