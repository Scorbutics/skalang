#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class StatementParser;
	struct Token;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherImport {
	public:
		~MatcherImport() = default;
		MatcherImport(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchImport(Script& input);
		ASTNodePtr matchExport(Script& input);
		ASTNodePtr matchNewImport(Script& input, const Token& importVarName, const Token& importNodeClass);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
