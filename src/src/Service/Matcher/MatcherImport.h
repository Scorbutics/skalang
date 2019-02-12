#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class StatementParser;
	struct Token;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherImport {
	public:
		~MatcherImport() = default;
		MatcherImport(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchImport(TokenReader& input);
		ASTNodePtr matchExport(TokenReader& input);
		ASTNodePtr matchNewImport(TokenReader& input, const Token& importVarName, const Token& importNodeClass);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}