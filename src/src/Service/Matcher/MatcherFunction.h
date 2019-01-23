#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class TokenReader;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherFunction {
	public:
		~MatcherFunction() = default;
		MatcherFunction(TokenReader& input, const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_input(input), m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchCall(ASTNodePtr identifierFunctionName);
		ASTNodePtr matchDeclaration();
	
	private:
		ASTNodePtr matchDeclarationBody();
		std::vector<ASTNodePtr> fillDeclarationParameters();
		ASTNodePtr matchDeclarationReturnType();
		ASTNodePtr matchDeclarationParameter();

		TokenReader& m_input;
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}