#include "Config/LoggerConfigLang.h"
#include "MatcherArray.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherArray)

ska::ASTNodePtr ska::MatcherArray::matchDeclaration(ScriptAST& input) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());

	auto arrayNode = std::vector<ASTNodePtr>{};
	auto isComma = true;
	while (isComma) {
		if (!input.reader().expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "array entry detected, reading expression : ";
			
			auto expression = input.expr(m_parser);
			if (expression != nullptr) {
				arrayNode.push_back(std::move(expression));
			}
			isComma = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
	}
    
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

	auto arrayOptionalType = ASTFactory::MakeEmptyNode();
	if (arrayNode.empty()) {
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		arrayOptionalType = m_matcherType.match(input.reader());
	}

	auto declarationNode = ASTFactory::MakeNode<ska::Operator::ARRAY_TYPE_DECLARATION>(ASTFactory::MakeNode<ska::Operator::ARRAY_DECLARATION>(std::move(arrayNode)), std::move(arrayOptionalType));
	auto event = ArrayTokenEvent{ *declarationNode, input, ArrayTokenEventType::DECLARATION };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

ska::ASTNodePtr ska::MatcherArray::matchUse(ScriptAST& input, ASTNodePtr nodeExpressionArrayAffected) {
	//Ensures array expression before the index access
	assert(nodeExpressionArrayAffected != nullptr);
	SLOG(ska::LogLevel::Debug) << "expression-array : " << *nodeExpressionArrayAffected << " with type " << nodeExpressionArrayAffected->type().value_or(Type{});	
    auto expressionEvent = ArrayTokenEvent{ *nodeExpressionArrayAffected, input, ArrayTokenEventType::EXPRESSION };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(expressionEvent);

    //Gets the index part with bracket syntax
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());
	auto indexNode = input.expr(m_parser);
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

	auto declarationNode = ASTFactory::MakeNode<ska::Operator::ARRAY_USE>(std::move(nodeExpressionArrayAffected), std::move(indexNode));
	
    //Notifies the outside that we use the array
    auto event = ArrayTokenEvent{ *declarationNode, input, ArrayTokenEventType::USE };
	m_parser.observable_priority_queue<ArrayTokenEvent>::notifyObservers(event);
	return declarationNode;
}

ska::ASTNodePtr ska::MatcherArray::match(ScriptAST& input, ExpressionStack& operands, char token, bool isDoingOperation) {
	switch (token) {
	case '[': {
		if (input.reader().canReadPrevious(1)) {
			auto lastToken = input.reader().readPrevious(1);
			const auto& value = lastToken.name();
			if (value != "=") {
				auto arrayNode = operands.popOperandIfNoOperator(isDoingOperation);
				if (arrayNode != nullptr) {
					SLOG(ska::LogLevel::Debug) << "\tArray begin use (previous token \"" << value << "\")";
					auto result = matchUse(input, std::move(arrayNode));
					SLOG(ska::LogLevel::Debug) << "\tArray end";
					return result;
				}
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
