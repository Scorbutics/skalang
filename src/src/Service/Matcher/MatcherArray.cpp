#include "Config/LoggerConfigLang.h"
#include "MatcherArray.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherArray)

ska::ASTNodePtr ska::MatcherArray::matchDeclaration(Script& input) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());

	auto arrayNode = std::vector<ASTNodePtr>{};
	auto isComma = true;
	while (isComma) {
		if (!input.expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "array entry detected, reading expression : ";
			
			auto expression = input.expr(m_parser);
			arrayNode.push_back(std::move(expression));
			isComma = input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
	}

    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());
	auto declarationNode = ASTFactory::MakeNode<ska::Operator::ARRAY_DECLARATION>(std::move(arrayNode));
	auto event = ArrayTokenEvent{ *declarationNode, input, ArrayTokenEventType::USE };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

ska::ASTNodePtr ska::MatcherArray::matchUse(Script& input, ASTNodePtr identifierArrayAffected) {
	//Ensures array expression before the index access
	SLOG(ska::LogLevel::Debug) << "expression-array : " << *identifierArrayAffected;	
    auto expressionEvent = ArrayTokenEvent{ *identifierArrayAffected, input, ArrayTokenEventType::EXPRESSION };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(expressionEvent);

    //Gets the index part with bracket syntax
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());
	auto indexNode = input.expr(m_parser);
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

	auto declarationNode = ASTFactory::MakeNode<ska::Operator::ARRAY_USE>(std::move(identifierArrayAffected), std::move(indexNode));
	
    //Notifies the outside that we use the array
    auto event = ArrayTokenEvent{ *declarationNode, input, ArrayTokenEventType::USE };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

ska::ASTNodePtr ska::MatcherArray::match(Script& input, ExpressionStack& operands, char token, bool isDoingOperation) {
	switch (token) {
	case '[': {
		if (input.canReadPrevious(1)) {
			auto lastToken = input.readPrevious(1);
			const auto& value = std::get<std::string>(lastToken.content());
			//TODO : handle multi dimensional arrays
			if (value != "=") {
				SLOG(ska::LogLevel::Debug) << "\tArray begin use";
				auto arrayNode = operands.popOperandIfNoOperator(isDoingOperation);
				if (arrayNode == nullptr) {
					arrayNode = ASTFactory::MakeEmptyNode();
					//throw std::runtime_error("invalid operator placement");
				}
				auto result = matchUse(input, std::move(arrayNode));
				SLOG(ska::LogLevel::Debug) << "\tArray end";
				return result;
			}
		}
		SLOG(ska::LogLevel::Debug) << "\tArray begin declare";
		auto reservedNode = matchDeclaration(input);
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
