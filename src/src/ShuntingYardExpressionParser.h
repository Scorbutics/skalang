#pragma once
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>
#include <Data/Events/EventDispatcher.h>

#include "ASTNodePtr.h"
#include "TokenReader.h"
#include "FunctionTokenEvent.h"
#include "VarTokenEvent.h"

#include "MatcherArray.h"

template<typename T, typename Container = std::deque<T>>
class iterable_stack
: public std::stack<T, Container>
{
    using std::stack<T, Container>::c;

public:

    // expose just the iterators of the underlying container
    auto begin() { return std::begin(c); }
    auto end() { return std::end(c); }

    auto begin() const { return std::begin(c); }
    auto end() const { return std::end(c); }
};

namespace ska {
	class ASTNode;
	class Parser;
	struct ReservedKeywordsPool;

	class ShuntingYardExpressionParser {

		using PopPredicate = std::function<int(const Token&)>;
		template <class T>
		using stack = iterable_stack<T>;

		static std::unordered_map<char, int> BuildPriorityMap() ;
		static std::unordered_map<char, int> PRIORITY_MAP;

	public:
		ShuntingYardExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, Parser& parser, TokenReader& input);
		ASTNodePtr parse();

	private:
		bool parseTokenExpression(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token, bool isDoingOperation);
        
        bool matchSymbol(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token);
		void matchArray(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token, bool isDoingOperation);
		void matchRange(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token, bool isDoingOperation);

        ASTNodePtr matchReserved();
		//ASTNodePtr matchArrayDeclaration();
		//ASTNodePtr matchArrayUse(ASTNodePtr identifierArrayAffected);
		ASTNodePtr matchFunctionCall(ASTNodePtr identifierFunctionName);
		ASTNodePtr matchFunctionDeclaration();
		ASTNodePtr matchFunctionDeclarationBody();
		std::vector<ASTNodePtr> fillFunctionDeclarationParameters();
		ASTNodePtr matchFunctionDeclarationReturnType();
		ASTNodePtr matchFunctionDeclarationParameter();
		ASTNodePtr matchObjectFieldAccess(ASTNodePtr objectAccessed);
		ASTNodePtr matchAffectation(Token identifierFieldAffected);
        
        static ASTNodePtr popOperandIfNoOperator(stack<ASTNodePtr>& operands, bool isMathOperator);

        bool isAtEndOfExpression() const;
		ASTNodePtr expression(stack<Token>& operators, stack<ASTNodePtr>& operands);
		bool checkLessPriorityToken(stack<Token>& operators, stack<ASTNodePtr>& operands, const char tokenChar) const;
		ASTNodePtr popUntil(stack<Token>& operators, stack<ASTNodePtr>& operands, PopPredicate predicate);
		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
		TokenReader& m_input;

		MatcherArray m_matcherArray;
	};

}
