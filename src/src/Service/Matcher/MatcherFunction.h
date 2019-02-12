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
		MatcherFunction(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchCall(TokenReader& input, ASTNodePtr identifierFunctionName);
		ASTNodePtr matchDeclaration(TokenReader& input);
	
	private:
		ASTNodePtr matchDeclarationBody(TokenReader& input);
		std::vector<ASTNodePtr> fillDeclarationParameters(TokenReader& input);
		ASTNodePtr matchDeclarationReturnType(TokenReader& input);
		ASTNodePtr matchDeclarationParameter(TokenReader& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}