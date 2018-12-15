#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class Parser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherReturn {
	public:
		~MatcherReturn() = default;
		MatcherReturn(TokenReader& input, const ReservedKeywordsPool& pool, Parser& parser) :
			m_input(input), m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match();
	
	private:
		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
	};
}