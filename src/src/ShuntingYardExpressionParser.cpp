#include <iostream>
#include "LoggerConfigLang.h"
#include "ShuntingYardExpressionParser.h"
#include "Parser.h"
#include "AST.h"
#include "ReservedKeywordsPool.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::ShuntingYardExpressionParser)

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

ska::ShuntingYardExpressionParser::ShuntingYardExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, Parser& parser, TokenReader& input) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_parser(parser),
	m_input(input),
	m_matcherArray(m_input, m_reservedKeywordsPool, m_parser),
	m_matcherFunction(m_input, m_reservedKeywordsPool, m_parser),
	m_matcherVar(m_input, m_reservedKeywordsPool, m_parser) {
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::parse() {
	auto data = expression_stack<Token, ASTNodePtr>{};
	return expression(data);
}

bool ska::ShuntingYardExpressionParser::parseTokenExpression(ExpressionStack& expressions, const Token& token, bool isDoingOperation) {

	switch (token.type()) {
	case TokenType::RESERVED: {
		SLOG(ska::LogLevel::Debug) << "\tPushing reserved";
		auto reservedNode = matchReserved();
		if (reservedNode != nullptr) {
			expressions.push(std::move(reservedNode));
			break;
		}
	}
	case TokenType::IDENTIFIER:
	case TokenType::STRING:
	case TokenType::DIGIT:
		SLOG(ska::LogLevel::Debug) << "\tPushing operand " << token;
		expressions.push(ASTNode::MakeLogicalNode(m_input.match(token.type())));
		break;

	case TokenType::ARRAY: {
		const auto& value = std::get<std::string>(token.content());
		expressions.push(m_matcherArray.match(expressions, value[0], isDoingOperation));
	} break;

	case TokenType::RANGE:
        matchRange(expressions, token, isDoingOperation);
		break;

	case TokenType::DOT_SYMBOL:
        //Field access only (digits are DIGIT token type, even real ones)
		expressions.push(matchObjectFieldAccess(expressions.popOperandIfNoOperator(isDoingOperation)));
        break;
		

	case TokenType::SYMBOL:
        return matchSymbol(expressions, token);

	default:
		error("Expected a symbol, a literal, an identifier or a reserved keyword, but got the token : " + token.name());
	}
    return false;
}

void ska::ShuntingYardExpressionParser::matchRange(ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
    const auto& value = std::get<std::string>(token.content());
    switch (value[0]) {
	case '(': {
		/*
		TODO:

		auto functionNameOperand = expressions.popOperandIfNoOperator(isDoingOperation);
		if (functionNameOperand != nullptr && functionNameOperand->type().has_value() && functionNameOperand->type() == ExpressionType::FUNCTION) {
			SLOG(ska::LogLevel::Debug) << "\tFunction call !";
			//We already pushed the identifier as an operand, but in fact it's a function call.
			//We have to pop it, then matching the function call as the new operand.
			expressions.push(m_matcherFunction.matchCall(std::move(functionNameOperand)));
		*/

		if (!expressions.emptyOperands() && (expressions.topOperand()->op() == Operator::FIELD_ACCESS ||
			(expressions.topOperand()->op() == Operator::LITERAL || expressions.topOperand()->op() == Operator::UNARY) && 
			expressions.topOperand()->tokenType() == TokenType::IDENTIFIER)) {
			SLOG(ska::LogLevel::Debug) << "\tFunction call !";
			//We already pushed the identifier as an operand, but in fact it's a function call.
			//We have to pop it, then matching the function call as the new operand.
			expressions.push(m_matcherFunction.matchCall(expressions.popOperandIfNoOperator(isDoingOperation)));
		} else {
			SLOG(ska::LogLevel::Debug) << "\tRange begin";
			expressions.push(Token{ m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) });
		}		
	} break;
    case ')': {
		auto tok = Token{ m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) };
		expressions.triggerRangePoping(std::move(tok));
		SLOG(ska::LogLevel::Debug) << "\tRange end";
    } break;

    default:
        error("Unexpected token (range type) : " + token.name());
    }
}

bool ska::ShuntingYardExpressionParser::matchSymbol(ExpressionStack& expressions, const Token& token) {
    const auto& value = std::get<std::string>(token.content());
    if(value == "=") {
		//TODO rework (this condition should be based on the type system and the semantic checker)
		//We must check that the token before the '=' is an lvalue.
		//Pops the variable name token (mentioned before the '=')
		expressions.replaceTopOperand(m_matcherVar.matchAffectation());
        return false;
    } 

    SLOG(ska::LogLevel::Debug) << "\t\tPushing operator symbol " << value;
    const auto shouldPopOperatorsStack = expressions.checkLessPriorityToken(value[0]);
    if (shouldPopOperatorsStack) {
        SLOG(ska::LogLevel::Debug) << "\tLess precedence, poping top operator before adding " << value;
        auto poped = 0u;
        auto popedToken = expressions.popUntil([&](const Token& t) {
            if (poped >= 1) {
                return -1;
            }
            poped++;
            return 0;
        });
        assert(popedToken != nullptr);
		expressions.push(std::move(popedToken));
    }
	expressions.push(Token{ m_input.match(TokenType::SYMBOL) });
    return true;
    
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchObjectFieldAccess(ASTNodePtr objectAccessed) {
	
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::METHOD_CALL_OPERATOR>());
    auto fieldAccessedIdentifier = m_input.match(TokenType::IDENTIFIER);

    return ASTNode::MakeNode<Operator::FIELD_ACCESS>(std::move(objectAccessed), ASTNode::MakeLogicalNode(fieldAccessedIdentifier));
}

bool ska::ShuntingYardExpressionParser::isAtEndOfExpression() const {
    return m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>()) || 
            m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>()) ||
            m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>()) ||
            m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>()); 
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::expression(ExpressionStack& expressions) {
	auto rangeCounter = 0;
    auto isDoingOperation = false;
    while (!isAtEndOfExpression() && rangeCounter >= 0) {
#ifdef SKA_LOG_SHUNTING_YARD_DETAILS
        PrintPtr(operands, "Operands");
		Print(operators, "Operators");
#endif
		rangeCounter += m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) ? 1 : 0;
		rangeCounter -= m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) ? 1 : 0;
        auto token = m_input.actual();
		if (rangeCounter >= 0) {
			isDoingOperation = parseTokenExpression(expressions, token, isDoingOperation);
		}
	}
	SLOG(ska::LogLevel::Debug) << "\tPoping everything";

	auto result = expressions.popUntil([](const auto& t) {
		return 0;
	});
	
	if (result != nullptr) {
		auto event = ExpressionTokenEvent{ *result };
		m_parser.Observable<ExpressionTokenEvent>::notifyObservers(event);
	}

	return result;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchReserved() {
	const auto& result = m_input.actual();

	switch(std::get<std::size_t>(result.content())) {
		case static_cast<std::size_t>(TokenGrammar::FUNCTION):
			return m_matcherFunction.matchDeclaration();

		default:
			return nullptr;
	}
}

void ska::ShuntingYardExpressionParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}



