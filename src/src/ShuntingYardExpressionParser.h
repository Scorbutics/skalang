#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <Data/Events/EventDispatcher.h>

#include "expression_stack.h"
#include "ASTNodePtr.h"
#include "TokenReader.h"
#include "FunctionTokenEvent.h"
#include "VarTokenEvent.h"

#include "MatcherArray.h"
#include "MatcherFunction.h"
#include "MatcherVar.h"

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
        
        bool matchSymbol(ExpressionStack& expressions, const Token& token);
		void matchRange(ExpressionStack& expressions, const Token& token, bool isDoingOperation);

        ASTNodePtr matchReserved();
		
		ASTNodePtr matchObjectFieldAccess(ASTNodePtr objectAccessed);      

        bool isAtEndOfExpression() const;
		
		ASTNodePtr expression(ExpressionStack& expressions);
		//bool checkLessPriorityToken(ExpressionStack& expressions, const char tokenChar) const;

		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
		TokenReader& m_input;

		MatcherArray m_matcherArray;
		MatcherFunction m_matcherFunction;
		MatcherVar m_matcherVar;
	};

}
