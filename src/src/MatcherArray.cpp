#include "LoggerConfigLang.h"
#include "MatcherArray.h"

#include "AST.h"
#include "Parser.h"
#include "TokenReader.h"
#include "ReservedKeywordsPool.h"
#include "BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::MatcherArray)

ska::ASTNodePtr ska::MatcherArray::matchDeclaration() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());

	auto arrayNode = std::vector<ASTNodePtr>{};
	auto isComma = true;
	while (isComma) {
		if (!m_input.expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "array entry detected, reading expression : ";
			
			auto expression = m_parser.expr();
			arrayNode.push_back(std::move(expression));
			isComma = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
	}

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());
	auto declarationNode = ASTNode::MakeNode<ska::Operator::ARRAY_DECLARATION>(std::move(arrayNode));
	auto event = ArrayTokenEvent{ *declarationNode, ArrayTokenEventType::USE };
	m_parser.Observable<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

ska::ASTNodePtr ska::MatcherArray::matchUse(ASTNodePtr identifierArrayAffected) {
	//Ensures array expression before the index access
    SLOG(ska::LogLevel::Debug) << "expression-array : " << *identifierArrayAffected;
    auto expressionEvent = ArrayTokenEvent{ *identifierArrayAffected, ArrayTokenEventType::EXPRESSION };
	m_parser.Observable<ArrayTokenEvent>::notifyObservers(expressionEvent);

    //Gets the index part with bracket syntax
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());
	auto indexNode = m_parser.expr();
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

	auto declarationNode = ASTNode::MakeNode<ska::Operator::ARRAY_USE>(std::move(identifierArrayAffected), std::move(indexNode));
	
    //Notifies the outside that we use the array
    auto event = ArrayTokenEvent{ *declarationNode, ArrayTokenEventType::USE };
	m_parser.Observable<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

