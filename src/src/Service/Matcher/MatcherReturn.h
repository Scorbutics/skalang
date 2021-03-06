#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class MatcherConverter;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherReturn {
	public:
		~MatcherReturn() = default;
		MatcherReturn(const ReservedKeywordsPool& pool, StatementParser& parser, MatcherConverter& matcherConverter) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherConverter(matcherConverter) {}
	
		ASTNodePtr match(ScriptAST& input, bool noevents = false);
	
	private:
		ASTNodePtr matchField(ScriptAST& input);
		ASTNodePtr matchCustomObject(ScriptAST& input, bool noevents);
		ASTNodePtr matchBuiltIn(ScriptAST& input, bool noevents);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherConverter& m_matcherConverter;
	};
}
