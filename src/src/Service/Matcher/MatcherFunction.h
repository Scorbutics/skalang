#pragma once
#include <memory>
#include <string>

#include "MatcherType.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherFunction {
	public:
		~MatcherFunction() = default;
		MatcherFunction(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherType(pool) {}
	
		ASTNodePtr matchCall(ScriptAST& input, ASTNodePtr identifierFunctionName);
		ASTNodePtr matchDeclaration(ScriptAST& input);
	
	private:
		ASTNodePtr matchDeclarationBody(ScriptAST& input);
		std::vector<ASTNodePtr> fillDeclarationParameters(ScriptAST& input);
		ASTNodePtr matchDeclarationReturnType(ScriptAST& input);
		ASTNodePtr matchDeclarationParameter(ScriptAST& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherType m_matcherType;
	};
}
