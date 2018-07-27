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

		static std::unordered_map<char, int> BuildPriorityMap() ;
		static std::unordered_map<char, int> PRIORITY_MAP;

	public:
		ShuntingYardExpressionParser(Parser& parser, TokenReader& input);
		std::unique_ptr<ska::ASTNode> parse();

	private:
		void clear();
		bool matchReserved();
		std::unique_ptr<ASTNode> expression();
		bool checkLessPriorityToken(const char tokenChar) const;
		std::unique_ptr<ASTNode> popUntil(PopPredicate predicate);
		static void error();

		TokenReader& m_input;

		Parser& m_parser;
		iterable_stack<Token> m_operators;
		iterable_stack<std::unique_ptr<ASTNode>> m_operands;
	};

}
