#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherFor {
	public:
		~MatcherFor() = default;
		MatcherFor(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match(Script& input);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
