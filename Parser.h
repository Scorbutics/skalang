#pragma once
#include <vector>
#include "Token.h"
#include "Scope.h"
#include "ShuntingYardExpressionParser.h"

namespace ska {

	class Parser {
		friend class ShuntingYardExpressionParser;
	public:
		Parser(TokenReader& input) ;

		std::pair<ASTNode, Scope> parse() ;

	private:
		void statement() ;

		void matchRange(const std::string& content) ;

		void matchReservedKeyword(const std::size_t keywordIndex) ;

		std::unique_ptr<ska::ASTNode> expr() ;

		static void error() ;

		void pushToken() ;

		void unbranch() ;

		void branch() ;

		std::unique_ptr<ska::ASTNode> optexpr(const Token& mustNotBe = Token{}) ;

		void optstatement(const Token& mustNotBe = Token{}) ;

		TokenReader& m_input;
		ShuntingYardExpressionParser m_shuntingYardParser;

		ASTNode* m_currentAst;
		Scope* m_currentScope;
	};
}
