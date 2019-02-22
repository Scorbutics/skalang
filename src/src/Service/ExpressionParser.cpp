#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "ExpressionParser.h"
#include "Service/StatementParser.h"
#include "NodeValue/AST.h"
#include "ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ExpressionParser)

//#define SKA_LOG_SHUNTING_YARD_DETAILS

template <class Container>
void Print(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		std::cout << it << " ";
	}
	std::cout << std::endl;
}

template <class Container>
void PrintPtr(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		if(it != nullptr) {
			std::cout << (*it) << " ";
		}
	}
	std::cout << std::endl;
}

ska::ExpressionParser::ExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, StatementParser& parser) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_parser(parser),
	m_matcherArray(m_reservedKeywordsPool, m_parser),
	m_matcherFunction(m_reservedKeywordsPool, m_parser),
	m_matcherVar(m_reservedKeywordsPool, m_parser),
	m_matcherImport(m_reservedKeywordsPool, m_parser) {
}

ska::ASTNodePtr ska::ExpressionParser::parse(Script& input) {
	auto data = expression_stack<Token, ASTNodePtr>{};
	return expression(input, data);
}

bool ska::ExpressionParser::parseTokenExpression(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {

	switch (token.type()) {
	case TokenType::RESERVED: {
		SLOG(ska::LogLevel::Debug) << "\tPushing reserved";
		auto reservedNode = matchReserved(input);
		if (reservedNode != nullptr) {
			expressions.push(std::move(reservedNode));
			break;
		}
	}
	case TokenType::BOOLEAN:
	case TokenType::STRING:
	case TokenType::DIGIT:
		SLOG(ska::LogLevel::Debug) << "\tPushing operand " << token;
		expressions.push(ASTFactory::MakeLogicalNode(input.match(token.type())));
		break;

	case TokenType::IDENTIFIER: {
		SLOG(ska::LogLevel::Debug) << "\tPushing var operand " << token;
		auto varNode = ASTFactory::MakeLogicalNode(input.match(token.type()));
		auto event = VarTokenEvent::MakeUse(*varNode);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
		expressions.push(std::move(varNode));
	} break;

	case TokenType::ARRAY: {
		const auto& value = std::get<std::string>(token.content());
		expressions.push(m_matcherArray.match(input, expressions, value[0], isDoingOperation));
	} break;

	case TokenType::RANGE:
        matchRange(input, expressions, token, isDoingOperation);
		break;

	case TokenType::DOT_SYMBOL: {
		//Field access only (digits are DIGIT token type, even real ones)
		auto expressionObject = expressions.popOperandIfNoOperator(isDoingOperation);
		if (expressionObject == nullptr) {
			throw std::runtime_error("invalid operator placement");
		}
		expressions.push(matchObjectFieldAccess(input, std::move(expressionObject)));
	} break;
		

	case TokenType::SYMBOL:
        return matchSymbol(input, expressions, token, isDoingOperation);

	default:
		error("Expected a symbol, a literal, an identifier or a reserved keyword, but got the token : " + token.name());
	}
    return false;
}

void ska::ExpressionParser::matchRange(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
    const auto& value = std::get<std::string>(token.content());
    switch (value[0]) {
	case '(':
		matchParenthesis(input, expressions, isDoingOperation);
	break;
    case ')': {
		expressions.push(Token{ input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) });
		
        auto endGrouping = false;
		auto groupParenthesisRange = [&endGrouping](const Token& t) {
            if(endGrouping) {
                return Group::FlowControl::STOP;
            }
            const auto& strValue = t.name();
			if (t.type() == TokenType::RANGE) {
                endGrouping = strValue == "(";
				return Group::FlowControl::IGNORE_AND_CONTINUE;
			}
			return Group::FlowControl::GROUP;
		};

		auto result = expressions.groupAndPop<ASTFactory>(std::move(groupParenthesisRange));
		if (result != nullptr) {
			expressions.push(std::move(result));
		}
		SLOG(ska::LogLevel::Debug) << "\tRange end";
    } break;

    default:
        error("Unexpected token (range type) : " + token.name());
    }
}

bool ska::ExpressionParser::matchSymbol(Script& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
    const auto& value = std::get<std::string>(token.content());
    if(value == "=") {
		//We must check that the token before the '=' is an lvalue : done in the semantic check pass.
		expressions.push(m_matcherVar.matchAffectation(input, expressions.popOperandIfNoOperator(isDoingOperation)));
        return false;
    }

    SLOG(ska::LogLevel::Debug) << "\t\tPushing operator symbol " << value;

    const auto shouldPopOperatorsStack = expressions.checkLessPriorityOperator(value[0]);
    if (shouldPopOperatorsStack) {
        SLOG(ska::LogLevel::Debug) << "\tLess precedence, poping first (top) operator before adding " << value;

		auto poped = false;
		auto expressionGroup = expressions.groupAndPop<ASTFactory>(Group::FirstOnly<Token>(poped));
		if (expressionGroup == nullptr) {
			error("void expression on affectation");
		}
		expressions.push(std::move(expressionGroup));
	} else {
		expressions.push(Token{ input.match(TokenType::SYMBOL) });
	}
    return true;
    
}

void ska::ExpressionParser::matchParenthesis(Script& input, ExpressionStack& expressions, bool isDoingOperation) {
	auto functionNameOperand = expressions.popOperandIfNoOperator(isDoingOperation);
	if (functionNameOperand != nullptr) {
		auto event = ExpressionTokenEvent{ *functionNameOperand };
		m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);

		if (functionNameOperand->type() == ExpressionType::FUNCTION) {
			SLOG(ska::LogLevel::Debug) << "\tFunction call !";
			//We already pushed the identifier as an operand, but in fact it's a function call.
			//We have to pop it, then matching the function call as the new operand.
			expressions.push(m_matcherFunction.matchCall(input, std::move(functionNameOperand)));
			return;
		}
	}

	SLOG(ska::LogLevel::Debug) << "\tRange begin";
	expressions.push(Token{ input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) });
}

ska::ASTNodePtr ska::ExpressionParser::matchObjectFieldAccess(Script& input, ASTNodePtr objectAccessed) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::METHOD_CALL_OPERATOR>());
    auto fieldAccessedIdentifier = input.match(TokenType::IDENTIFIER);

    return ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(objectAccessed), ASTFactory::MakeLogicalNode(fieldAccessedIdentifier));
}

bool ska::ExpressionParser::isAtEndOfExpression(Script& input) const {
    return input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>()) || 
            input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>()) ||
            input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>()) ||
            input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>()); 
}

ska::ASTNodePtr ska::ExpressionParser::expression(Script& input, ExpressionStack& expressions) {
	auto rangeCounter = 0;
    auto isDoingOperation = false;
    while (!isAtEndOfExpression(input) && rangeCounter >= 0) {
#ifdef SKA_LOG_SHUNTING_YARD_DETAILS
        PrintPtr(operands, "Operands");
		Print(operators, "Operators");
#endif
		rangeCounter += input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) ? 1 : 0;
		rangeCounter -= input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) ? 1 : 0;
        auto token = input.actual();
		if (rangeCounter >= 0) {
			isDoingOperation = parseTokenExpression(input, expressions, token, isDoingOperation);
		}
	}
	SLOG(ska::LogLevel::Debug) << "\tPoping everything";

	auto expressionGroup = expressions.template groupAndPop <ASTFactory>(Group::All<Token>);
	if (expressionGroup != nullptr) {
		auto event = ExpressionTokenEvent { *expressionGroup };
		m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);
	}

	return expressionGroup;

}

ska::ASTNodePtr ska::ExpressionParser::matchReserved(Script& input) {
	const auto& result = input.actual();

	switch(std::get<std::size_t>(result.content())) {
		case static_cast<std::size_t>(TokenGrammar::FUNCTION):
			return m_matcherFunction.matchDeclaration(input);
		case static_cast<std::size_t>(TokenGrammar::IMPORT) :
			return m_matcherImport.matchImport(input);
		default:
			return nullptr;
	}
}

void ska::ExpressionParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}



