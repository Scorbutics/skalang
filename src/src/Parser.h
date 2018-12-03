#pragma once
#include <vector>
#include <Data/Events/EventDispatcher.h>

#include "Token.h"
#include "ShuntingYardExpressionParser.h"

#include "ForTokenEvent.h"
#include "IfElseTokenEvent.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ExpressionTokenEvent.h"
#include "ReturnTokenEvent.h"
#include "ArrayTokenEvent.h"

namespace ska {
	struct ReservedKeywordsPool;

	class Parser :
	    public EventDispatcher<
            ForTokenEvent,
            IfElseTokenEvent,
            VarTokenEvent,
	    	FunctionTokenEvent,
            BlockTokenEvent,
		    ExpressionTokenEvent,
            ReturnTokenEvent,
			ArrayTokenEvent
	    > {

		friend class ShuntingYardExpressionParser;
		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
	public:
		Parser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input);
		ASTNodePtr parse();

	private:
		ASTNodePtr statement();
        ASTNodePtr optstatement(const Token& mustNotBe = Token{});

		ASTNodePtr matchReservedKeyword(const std::size_t keywordIndex);

        ASTNodePtr matchBlock(const std::string& content);
        ASTNodePtr matchForKeyword();
        ASTNodePtr matchIfOrIfElseKeyword();
        ASTNodePtr matchVarKeyword();
        ASTNodePtr matchReturnKeyword();
        ASTNodePtr matchExpressionStatement();

		ASTNodePtr expr();
		ASTNodePtr optexpr(const Token& mustNotBe = Token{});

		static void error(const std::string& message);

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ShuntingYardExpressionParser m_shuntingYardParser;
	};
}
