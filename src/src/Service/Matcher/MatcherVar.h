#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherVar {
	public:
		~MatcherVar() = default;
		MatcherVar(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchDeclaration(Script& input);
		ASTNodePtr matchAffectation(Script& input, ASTNodePtr varAffectedNode);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
