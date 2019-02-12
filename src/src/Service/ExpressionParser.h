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
#include "Matcher/MatcherImport.h"

namespace ska {
	class ASTNode;
	class StatementParser;
	struct ReservedKeywordsPool;

	class ExpressionParser {

		using PopPredicate = std::function<int(const Token&)>;
		using ExpressionStack = expression_stack<Token, ASTNodePtr>;

	public:
		ExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, StatementParser& parser);
		ASTNodePtr parse(TokenReader& input);

	private:
		bool parseTokenExpression(TokenReader& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
        
        bool matchSymbol(TokenReader& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchRange(TokenReader& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchParenthesis(TokenReader& input, ExpressionStack& expressions, bool isDoingOperation);

        ASTNodePtr matchReserved(TokenReader& input);
		ASTNodePtr matchObjectFieldAccess(TokenReader& input, ASTNodePtr objectAccessed);      

        bool isAtEndOfExpression(TokenReader& input) const;
		
		ASTNodePtr expression(TokenReader& input, ExpressionStack& expressions);

		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;

		MatcherArray m_matcherArray;
		MatcherFunction m_matcherFunction;
		MatcherVar m_matcherVar;
		MatcherImport m_matcherImport;
	};

}
