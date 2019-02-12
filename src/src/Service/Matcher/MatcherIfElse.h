#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherIfElse {
	public:
		~MatcherIfElse() = default;
		MatcherIfElse(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match(TokenReader& input);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}