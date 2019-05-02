#include "Config/LoggerConfigLang.h"
#include "MatcherType.h"

#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherType)

ska::ASTNodePtr ska::MatcherType::match(TokenReader& input) {
	const auto& typeDelimiterToken = m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>();

	bool isBuiltIn = true;
	auto nodes = std::vector<ASTNodePtr> {};
	if (input.expect(TokenType::IDENTIFIER)) {
		auto typeNamespaceToken = input.match(TokenType::IDENTIFIER);
		//Handles script namespace
		auto complexTypeToken = Token{};
		if (input.expect(typeDelimiterToken)) {
			input.match(typeDelimiterToken);
			input.match(typeDelimiterToken);
			complexTypeToken = input.match(TokenType::IDENTIFIER);
		}
		nodes.push_back(ASTFactory::MakeLogicalNode(std::move(typeNamespaceToken), complexTypeToken.empty() ? nullptr : ASTFactory::MakeLogicalNode(std::move(complexTypeToken))));
		isBuiltIn = false;
	} else {
		nodes.push_back(ASTFactory::MakeLogicalNode(input.match(TokenType::RESERVED)));
	}

	//handle objects
	if (input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>())) {
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

		if (isBuiltIn) {
			throw std::runtime_error("syntax error : \"" + nodes[0]->name() + "\" is used as a function-object but is a built-in type");
		}

		nodes.push_back(ASTFactory::MakeLogicalNode(Token{ "", TokenType::RANGE }));
	} else {
		nodes.push_back(ASTFactory::MakeEmptyNode());
	}

	//handle arrays
	if (input.expect(TokenType::ARRAY)) {
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

		nodes.push_back(ASTFactory::MakeLogicalNode(Token{ "", TokenType::ARRAY }));
	} else {
		nodes.push_back(ASTFactory::MakeEmptyNode());
	}

	assert(nodes.size() == 3);
	return ASTFactory::MakeNode<Operator::TYPE>(std::move(nodes));
}