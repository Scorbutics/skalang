#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

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
		ASTNodePtr parse(Script& input);

	private:
		bool parseTokenExpression(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
        
        bool matchSymbol(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchRange(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		void matchParenthesis(Script& input, ExpressionStack& expressions, bool isDoingOperation);

        ASTNodePtr matchReserved(Script& input);
		ASTNodePtr matchObjectFieldAccess(Script& input, ASTNodePtr objectAccessed);      

        bool isAtEndOfExpression(Script& input) const;
		
		ASTNodePtr expression(Script& input, ExpressionStack& expressions);

		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;

		MatcherArray m_matcherArray;
		MatcherFunction m_matcherFunction;
		MatcherVar m_matcherVar;
		MatcherImport m_matcherImport;
	};

}
