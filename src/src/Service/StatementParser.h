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
#include "Event/ImportTokenEvent.h"
#include "Event/BridgeTokenEvent.h"

#include "Matcher/MatcherBlock.h"
#include "Matcher/MatcherFor.h"
#include "Matcher/MatcherIfElse.h"
#include "Matcher/MatcherVar.h"
#include "Matcher/MatcherReturn.h"

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class ExpressionParser;
	
	class StatementParser :
	    public EventDispatcher<
            ForTokenEvent,
            IfElseTokenEvent,
            VarTokenEvent,
	    	FunctionTokenEvent,
            BlockTokenEvent,
		    ExpressionTokenEvent,
            ReturnTokenEvent,
			ArrayTokenEvent,
			ImportTokenEvent,
			BridgeTokenEvent
	    > {

		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class Script;
	public:
		StatementParser(const ReservedKeywordsPool& reservedKeywordsPool);
		ASTNodePtr subParse(std::ifstream& file);

	private:
		ASTNodePtr parse(Script& input);

		ASTNodePtr statement(Script& input);
        ASTNodePtr optstatement(Script& input, const Token& mustNotBe = Token{});

		ASTNodePtr expr(Script& input);
		ASTNodePtr optexpr(Script& input, const Token& mustNotBe = Token{});

		ASTNodePtr matchExpressionStatement(Script& input);
		ASTNodePtr matchReservedKeyword(Script& input, const std::size_t keywordIndex);
		static void error(const std::string& message);

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
