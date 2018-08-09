#include <iostream>
#include "ShuntingYardExpressionParser.h"
#include "Parser.h"
#include "AST.h"
#include "ReservedKeywordsPool.h"

template <class Container>
void Print(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		std::cout << it.asString() << " ";
	}
	std::cout << std::endl;
}

template <class Container>
void PrintPtr(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		if(it != nullptr) {
			std::cout << it->asString() << " ";
		}
	}
	std::cout << std::endl;
}

std::unordered_map<char, int> ska::ShuntingYardExpressionParser::BuildPriorityMap() {
	auto result = std::unordered_map<char, int>{};

	result.emplace('+', 100);
	result.emplace('-', 100);
	result.emplace('*', 200);
	result.emplace('/', 200);

	return result;
}

std::unordered_map<char, int> ska::ShuntingYardExpressionParser::PRIORITY_MAP = BuildPriorityMap();

ska::ShuntingYardExpressionParser::ShuntingYardExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, Parser& parser, TokenReader& input) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_parser(parser),
	m_input(input) {
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::parse() {
	auto operators = stack<Token>{};
	auto operands = stack<std::unique_ptr<ASTNode>> {};
	return expression(operators, operands);
}

bool ska::ShuntingYardExpressionParser::parseTokenExpression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& token, Token& lastToken) {

	switch (token.type()) {
		case TokenType::RESERVED: {
			std::cout << "\tPushing reserved" << std::endl;
			auto reservedNode = matchReserved();
			if (reservedNode != nullptr) {
				operands.push(std::move(reservedNode));
				break;
			}
		}
		case TokenType::IDENTIFIER:
			lastToken = token;
		case TokenType::STRING:
		case TokenType::DIGIT:
			std::cout << "\tPushing operand " << token.asString() << std::endl;
			operands.push(std::make_unique<ASTNode>(m_input.match(token.type())));
			return true;

		case TokenType::RANGE: {
			const auto& value = std::get<std::string>(token.content());
			switch (value[0]) {
			case '(':

				if (!lastToken.empty()) {
					std::cout << "\tFunction call !" << std::endl;
					//We already pushed the identifier as an operand, but in fact it's a function call.
					//We have to pop it, then matching the function call as the new operand.
					operands.pop();
					operands.push(matchFunctionCall(lastToken));
				} else {
					std::cout << "\tRange begin" << std::endl;
					operators.emplace(m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()));
				}
				break;
			case ')': {
				operators.emplace(m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()));
				std::cout << "\tRange end" << std::endl;
				auto rangeOperandResult = popUntil(operators, operands, [](const Token& t) {
					const auto& strValue = t.asString();
					if (t.type() == TokenType::RANGE) {
						return (strValue.empty() || strValue[0] == '(') ? -1 : 1;
					}
					std::cout << "\t\tPoping " << strValue << std::endl;
					return 0;
				});
				if (rangeOperandResult != nullptr) {
					operands.push(std::move(rangeOperandResult));
				}
				if (!operators.empty()) {
					operators.pop();
				}
			}
			break;

			default:
				error();
			}

		}
		break;

		case TokenType::SYMBOL: {
			const auto& value = std::get<std::string>(token.content());
			std::cout << "\tPushing operator symbol " << value << std::endl;
			const auto shouldPopOperatorsStack = checkLessPriorityToken(operators, operands, value[0]);
			auto operatorTop = operators.empty() ? Token{} : operators.top();
			if (shouldPopOperatorsStack) {
				std::cout << "\tLess precedence, poping " << operatorTop.asString() << " before adding " << value << std::endl;
				auto poped = 0u;
				auto popedToken = popUntil(operators, operands, [&](const Token& t) {
					if (poped >= 1) {
						return -1;
					}
					poped++;
					return 0;
				});
				assert(popedToken != nullptr);
				operands.push(std::move(popedToken));
			}
			operators.emplace(m_input.match(TokenType::SYMBOL));
		}
		break;

	default:
		error();
	}

	lastToken = Token{};
	return false;
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::matchFunctionCall(Token identifierFunctionName) {
	auto functionCallNode = std::make_unique<ska::ASTNode>(Operator::FUNCTION_CALL, std::move(identifierFunctionName));

	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	while (!m_input.expect(endParametersToken)) {
		auto expressionOpt = parse();
		if (expressionOpt != nullptr) {
			functionCallNode->add(std::move(expressionOpt));
		} else {
			break;
		}
	}
	return functionCallNode;
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::expression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands) {
	auto node = ASTNode{};
	auto rangeCounter = 0;
	auto lastToken = Token{};
	while (!m_input.expect(Token{ ";", TokenType::SYMBOL }) && !m_input.expect(Token{ ",", TokenType::SYMBOL }) && rangeCounter >= 0) {
		//PrintPtr(operands, "Operands");
		//Print(operators, "Operators");
		rangeCounter += m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) ? 1 : 0;
		rangeCounter -= m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) ? 1 : 0;
		auto token = m_input.actual();
		if (rangeCounter >= 0) {
			parseTokenExpression(operators, operands, token, lastToken);
			if (!operands.empty() && operands.top()->op == Operator::FUNCTION_CALL) {
				rangeCounter = 0;
			}
		}

	}
	std::cout << "\tPoping everything" << std::endl;
	auto result = popUntil(operators, operands, [](const auto& t) {
		return 0;
	});
	return result;
}

bool ska::ShuntingYardExpressionParser::checkLessPriorityToken(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const char tokenChar) const {
	const auto& topOperatorContent = (operators.empty() || operators.top().asString().empty()) ? '\0' : operators.top().asString()[0];
	return PRIORITY_MAP.find(tokenChar) != PRIORITY_MAP.end() &&
			PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
			PRIORITY_MAP.at(tokenChar) < PRIORITY_MAP.at(topOperatorContent);
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::matchReserved() {
	const auto& result = m_input.actual();

	switch(std::get<std::size_t>(result.content())) {
		case static_cast<std::size_t>(TokenGrammar::FUNCTION):
			return matchFunctionDeclaration();

		default:
			return nullptr;
	}
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::matchFunctionDeclaration() {
	auto functionDeclarationNode = std::make_unique<ska::ASTNode>(Operator::FUNCTION_DECLARATION);

	std::cout << "function declaration" << std::endl;
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!m_input.expect(TokenType::SYMBOL)) {
			std::cout << "parameter detected, reading identifier : ";
			const auto& id = m_input.match(TokenType::IDENTIFIER);
			std::cout << id.asString() << std::endl;
			functionDeclarationNode->add(std::make_unique<ASTNode>(id));
			isComma = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}

	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	std::cout << "reading function statement" << std::endl;
	functionDeclarationNode->add(m_parser.statement());
	std::cout << "function read." << std::endl;

	auto event = FunctionTokenEvent {*functionDeclarationNode};
    Observable<FunctionTokenEvent>::notifyObservers(event);

	return functionDeclarationNode;
}

std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::popUntil(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, PopPredicate predicate) {
	auto currentNode = std::unique_ptr<ASTNode> {};
	auto nextNode = std::unique_ptr<ASTNode> {};

	if (operators.empty() && !operands.empty()) {
		auto result = std::move(operands.top());
		operands.pop();
		return result;
	}

	while(true) {
		if (operators.empty() || operands.empty()) {
			break;
		}

		const auto op = operators.top();
		const auto analyzeToken = predicate(op);
		//Flow control loop predicate by token
		if(analyzeToken < 0) {
			break;
		} else if (analyzeToken > 0) {
			operators.pop();
			continue;
		}
		operators.pop();

		currentNode = std::make_unique<ASTNode>(Operator::BINARY, op);

		auto rightOperand = std::move(operands.top());
		operands.pop();

		if(nextNode != nullptr) {
			currentNode->add(std::move(nextNode));
		} else if(!operands.empty()) {
			currentNode->add(std::move(operands.top())); operands.pop();
		} else {
			//Unary operator ?
			//currentNode->add(std::make_unique<ASTNode>(Token{ "0", TokenType::DIGIT }));
			currentNode->op = Operator::UNARY;
		}
		currentNode->add(std::move(rightOperand));

		nextNode = std::move(currentNode);
		std::cout << "\t\tPoped " << op.asString() << std::endl;//<< " with " <<  nextNode->left->token.asString() << " and " << nextNode->right->token.asString() << std::endl;
	}

	return (currentNode == nullptr  && nextNode == nullptr) ? nullptr : std::move(nextNode);
}

void ska::ShuntingYardExpressionParser::error() {
	throw std::runtime_error("syntax error");
}


