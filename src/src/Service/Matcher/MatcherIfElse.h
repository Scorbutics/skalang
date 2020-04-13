#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class MatcherBlock;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherIfElse {
	public:
		~MatcherIfElse() = default;
		MatcherIfElse(const ReservedKeywordsPool& pool, StatementParser& parser, MatcherBlock& matcherBlock) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherBlock(matcherBlock) {}
	
		ASTNodePtr match(ScriptAST& input);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherBlock& m_matcherBlock;
	};
}
