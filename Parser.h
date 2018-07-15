#pragma once
#include <vector>
#include "Token.h"
#include "Scope.h"
#include "ShuntingYardExpressionParser.h"

namespace ska {
	
	class Parser {
		friend class ShuntingYardExpressionParser;
	public:
		Parser(std::vector<Token> input) ;

		std::pair<ASTNode, Scope> parse() ;

	private:
		void statement() ;

		void matchRange(const std::string& content) ;

		void matchReservedKeyword(const std::size_t keywordIndex) ;

		std::unique_ptr<ska::ASTNode> expr() ;

		static void error() ;

		const Token& match(const TokenType type) ;

		Token match(Token t) ;

		void pushToken() ;

		void unbranch() ;

		void branch() ;

		void nextToken() ;

		std::unique_ptr<ska::ASTNode> optexpr(const Token& mustNotBe = Token{}) ;

		void optstatement(const Token& mustNotBe = Token{}) ;

		std::vector<Token> m_input;
		ShuntingYardExpressionParser m_shuntingYardParser;
		std::size_t m_lookAheadIndex = 0;
		const Token* m_lookAhead = nullptr;
		
		ASTNode* m_currentAst;
		Scope* m_currentScope;
	};
}