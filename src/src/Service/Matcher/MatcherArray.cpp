#include "Config/LoggerConfigLang.h"
#include "MatcherArray.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherArray)

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

ska::ASTNodePtr ska::MatcherArray::match(ExpressionStack& operands, char token, bool isDoingOperation) {
	switch (token) {
	case '[': {
		if (m_input.canReadPrevious(1)) {
			auto lastToken = m_input.readPrevious(1);
			const auto& value = std::get<std::string>(lastToken.content());
			//TODO : handle multi dimensional arrays
			if (value != "=") {
				SLOG(ska::LogLevel::Debug) << "\tArray begin use";
				auto arrayNode = operands.popOperandIfNoOperator(isDoingOperation);
				if (arrayNode == nullptr) {
					throw std::runtime_error("invalid operator placement");
				}
				auto result = matchUse(std::move(arrayNode));
				SLOG(ska::LogLevel::Debug) << "\tArray end";
				return result;
			}
		}
		SLOG(ska::LogLevel::Debug) << "\tArray begin declare";
		auto reservedNode = matchDeclaration();
		if (reservedNode == nullptr) {
			throw std::runtime_error("syntax error : invalid array declaration");
		}
		SLOG(ska::LogLevel::Debug) << "\tArray end";
		return reservedNode;

	} break;

	default:
		throw std::runtime_error("syntax error : unsupported array symbol");
	}
}
