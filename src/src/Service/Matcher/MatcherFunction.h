#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherFunction {
	public:
		~MatcherFunction() = default;
		MatcherFunction(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchCall(Script& input, ASTNodePtr identifierFunctionName);
		ASTNodePtr matchDeclaration(Script& input);
	
	private:
		ASTNodePtr matchDeclarationBody(Script& input);
		std::vector<ASTNodePtr> fillDeclarationParameters(Script& input);
		ASTNodePtr matchDeclarationReturnType(Script& input);
		ASTNodePtr matchDeclarationParameter(Script& input);
		ASTNodePtr matchTypeNode(Script& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
