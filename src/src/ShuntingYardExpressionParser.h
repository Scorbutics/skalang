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

		bool parseTokenExpression(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token, Token& lastToken);

	private:
		ASTNodePtr matchReserved();
		ASTNodePtr matchFunctionCall(ASTNodePtr identifierFunctionName);
		ASTNodePtr matchFunctionDeclaration();
		ASTNodePtr matchFunctionDeclarationBody();
		void fillFunctionDeclarationParameters(ASTNode& functionDeclarationNode);
		ASTNodePtr matchFunctionDeclarationReturnType();
		ASTNodePtr matchFunctionDeclarationParameter();
		ASTNodePtr matchObjectFieldAccess(Token objectAccessed);
		ASTNodePtr matchAffectation(Token identifierFieldAffected);
        
        bool isAtEndOfExpression() const;
		ASTNodePtr expression(stack<Token>& operators, stack<ASTNodePtr>& operands);
		bool checkLessPriorityToken(stack<Token>& operators, stack<ASTNodePtr>& operands, const char tokenChar) const;
		ASTNodePtr popUntil(stack<Token>& operators, stack<ASTNodePtr>& operands, PopPredicate predicate);
		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
		TokenReader& m_input;
	};

}
