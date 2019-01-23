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
		MatcherBlock(TokenReader& input, const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_input(input), m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match(const std::string& content);
	
	private:
		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}