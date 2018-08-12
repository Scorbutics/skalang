#pragma once
#include <vector>
#include <Data/Events/EventDispatcher.h>

#include "Token.h"
#include "Scope.h"
#include "ShuntingYardExpressionParser.h"

#include "ForTokenEvent.h"
#include "IfElseTokenEvent.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"

namespace ska {
	struct ReservedKeywordsPool;

	class Parser :
	    public EventDispatcher<
            	ForTokenEvent,
            	IfElseTokenEvent,
            	VarTokenEvent,
            	BlockTokenEvent
	    > {

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

		static void error(const std::string& message);

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ShuntingYardExpressionParser m_shuntingYardParser;
	};
}
