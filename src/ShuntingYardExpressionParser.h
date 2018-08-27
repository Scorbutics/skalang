#pragma once
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>
#include <Data/Events/EventDispatcher.h>

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
		std::unique_ptr<ska::ASTNode> parse();

		bool parseTokenExpression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& token, Token& lastToken);

	private:
		std::unique_ptr<ASTNode> matchReserved();
		std::unique_ptr<ASTNode> matchFunctionCall(std::unique_ptr<ASTNode> identifierFunctionName);
		std::unique_ptr<ASTNode> matchFunctionDeclaration();
		std::unique_ptr<ASTNode> matchFunctionDeclarationParameter();
		std::unique_ptr<ASTNode> matchObjectFieldAccess(Token objectAccessed);
		std::unique_ptr<ASTNode> matchAffectation(Token identifierFieldAffected);
        
        bool isAtEndOfExpression() const;
		std::unique_ptr<ASTNode> expression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands);
		bool checkLessPriorityToken(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const char tokenChar) const;
		std::unique_ptr<ASTNode> popUntil(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, PopPredicate predicate);
		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
		TokenReader& m_input;
	};

}
