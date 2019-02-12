#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherBlock {
	public:
		~MatcherBlock() = default;
		MatcherBlock(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match(TokenReader& input, const std::string& content);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}