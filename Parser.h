#pragma once
#include <vector>
#include "Token.h"
#include "Scope.h"
#include "ShuntingYardExpressionParser.h"

namespace ska {

	class Parser {
		friend class ShuntingYardExpressionParser;
		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
	public:
		Parser(TokenReader& input) ;

		std::pair<ASTNodePtr, Scope> parse() ;

	private:
		ASTNodePtr statement() ;

		ASTNodePtr matchBlock(const std::string& content) ;

		ASTNodePtr matchReservedKeyword(const std::size_t keywordIndex) ;

		ASTNodePtr expr() ;

		static void error() ;


		ASTNodePtr optexpr(const Token& mustNotBe = Token{}) ;

		ASTNodePtr optstatement(const Token& mustNotBe = Token{}) ;

		TokenReader& m_input;
		ShuntingYardExpressionParser m_shuntingYardParser;
	};
}
