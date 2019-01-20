#pragma once
#include <vector>
#include <Data/Events/EventDispatcher.h>

#include "NodeValue/Token.h"
#include "Service/ExpressionParser.h"

#include "Event/ForTokenEvent.h"
#include "Event/IfElseTokenEvent.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"

#include "Matcher/MatcherBlock.h"
#include "Matcher/MatcherFor.h"
#include "Matcher/MatcherIfElse.h"
#include "Matcher/MatcherVar.h"
#include "Matcher/MatcherReturn.h"

namespace ska {
	struct ReservedKeywordsPool;

	class StatementParser :
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

		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
	public:
		StatementParser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input);
		ASTNodePtr parse();

		ASTNodePtr statement();
        ASTNodePtr optstatement(const Token& mustNotBe = Token{});

		ASTNodePtr matchReservedKeyword(const std::size_t keywordIndex);
        ASTNodePtr matchExpressionStatement();

		ASTNodePtr expr();
		ASTNodePtr optexpr(const Token& mustNotBe = Token{});

		ASTNodePtr subParse(std::ifstream& file);
	
	private:
		static void error(const std::string& message);

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ExpressionParser m_expressionParser;
		
		MatcherBlock m_matcherBlock;
		MatcherFor m_matcherFor;
		MatcherVar m_matcherVar;
		MatcherIfElse m_matcherIfElse;
		MatcherReturn m_matcherReturn;
		MatcherImport m_matcherImport;
	};
}
