#pragma once
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>

#include "TokenReader.h"

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

	class ShuntingYardExpressionParser {
		using PopPredicate = std::function<int(const Token&)>;
		template <class T>
		using stack = iterable_stack<T>;

		static std::unordered_map<char, int> BuildPriorityMap() ;
		static std::unordered_map<char, int> PRIORITY_MAP;

	public:
		ShuntingYardExpressionParser(Parser& parser, TokenReader& input);
		std::unique_ptr<ska::ASTNode> parse(const Token& expectedEnd = Token{ ";", TokenType::SYMBOL });

		bool parseTokenExpression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& token, const Token& expectedEnd);

	private:
		bool matchReserved();
		std::unique_ptr<ASTNode> expression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& expectedEnd);
		bool checkLessPriorityToken(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const char tokenChar) const;
		std::unique_ptr<ASTNode> popUntil(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, PopPredicate predicate);
		static void error();

		TokenReader& m_input;

		Parser& m_parser;
	};

}
