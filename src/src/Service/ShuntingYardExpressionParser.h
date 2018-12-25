#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <Data/Events/EventDispatcher.h>

#include "Container/expression_stack.h"
#include "NodeValue/ASTNodePtr.h"
#include "TokenReader.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/VarTokenEvent.h"

#include "Matcher/MatcherArray.h"
#include "Matcher/MatcherFunction.h"
#include "Matcher/MatcherVar.h"

namespace ska {
	class ASTNode;
	class Parser;
	struct ReservedKeywordsPool;

	class ShuntingYardExpressionParser {

		using PopPredicate = std::function<int(const Token&)>;
		using ExpressionStack = expression_stack<Token, ASTNodePtr>;

	public:
		ShuntingYardExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, Parser& parser, TokenReader& input);
		ASTNodePtr parse();

	private:
		bool parseTokenExpression(ExpressionStack& expressions, const Token& token, bool isDoingOperation);
        
        bool matchSymbol(ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchRange(ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchParenthesis(ExpressionStack& expressions, bool isDoingOperation);

        ASTNodePtr matchReserved();
		ASTNodePtr matchObjectFieldAccess(ASTNodePtr objectAccessed);      

        bool isAtEndOfExpression() const;
		
		ASTNodePtr expression(ExpressionStack& expressions);

		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
		TokenReader& m_input;

		MatcherArray m_matcherArray;
		MatcherFunction m_matcherFunction;
		MatcherVar m_matcherVar;
	};

}
