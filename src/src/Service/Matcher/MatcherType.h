#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherType {
	public:
		~MatcherType() = default;
		MatcherType(const ReservedKeywordsPool& pool) :
			m_reservedKeywordsPool(pool) {}
	
		ASTNodePtr match(TokenReader& input);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
	};
}
