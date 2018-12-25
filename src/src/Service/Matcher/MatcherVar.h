#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class Parser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherVar {
	public:
		~MatcherVar() = default;
		MatcherVar(TokenReader& input, const ReservedKeywordsPool& pool, Parser& parser) :
			m_input(input), m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchDeclaration();
		ASTNodePtr matchAffectation(ASTNodePtr varAffectedNode);
	
	private:
		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		Parser& m_parser;
	};
}