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

		ska::ShuntingYardExpressionParser::ShuntingYardExpressionParser(Parser& parser, TokenReader& input) :
			m_parser(parser),
			m_input(input) {
		}

		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::parse(const Token& expectedEnd) {
			auto operators = stack<Token>{};
			auto operands = stack<std::unique_ptr<ASTNode>> {};
			return expression(operators, operands, expectedEnd);
		}

		bool ska::ShuntingYardExpressionParser::parseTokenExpression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& token, const Token& expectedEnd) {
			switch (token.type()) {
				case TokenType::RESERVED:
					std::cout << "\tPushing reserved" << std::endl;
					if (matchReserved()) {
						break;
					}
				case TokenType::STRING:
				case TokenType::IDENTIFIER:
				case TokenType::DIGIT:
					std::cout << "\tPushing operand " << token.asString() << std::endl;
					operands.push(std::make_unique<ASTNode>(m_input.match(token.type())));
					break;

				case TokenType::RANGE: {
					const auto& value = std::get<std::string>(token.content());
					switch (value[0]) {
					case '(':
						std::cout << "\tRange begin" << std::endl;
						operators.emplace(m_input.match(Token{ "(", TokenType::RANGE }));
						break;
					case ')': {
						operators.emplace(m_input.match(Token{ ")", TokenType::RANGE }));
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
						if (expectedEnd == Token{ ")", TokenType::RANGE }) {
							return false;
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
			return true;
		}

		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::expression(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, const Token& expectedEnd) {
			auto node = ASTNode{};

			//std::cout << "Shunting Yards steps :" << std::endl;
			while(!m_input.empty() && !m_input.expect(expectedEnd)) {
				//PrintPtr(operands, "Operands");
				//Print(operators, "Operators");

				parseTokenExpression(operators, operands, m_input.actual(), expectedEnd);

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

		bool ska::ShuntingYardExpressionParser::matchReserved() {
			const auto& result = m_input.actual();

			switch(std::get<std::size_t>(result.content())) {
				case ReservedKeywords::FUNCTION: {
						m_parser.matchFunction();
					}
					return true;

				default:
					return false;
			}
		}

		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::popUntil(stack<Token>& operators, stack<std::unique_ptr<ASTNode>>& operands, PopPredicate predicate) {
			auto currentNode = std::unique_ptr<ASTNode> {};
			auto nextNode = std::unique_ptr<ASTNode> {};

			while(true) {
				if(operators.empty() || operands.empty()) {
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

				currentNode = std::make_unique<ASTNode>();
				currentNode->token = op;
				currentNode->right = std::move(operands.top()); operands.pop();

				if(nextNode != nullptr) {
					currentNode->left = std::move(nextNode);
				}else {
					currentNode->left = std::move(operands.top()); operands.pop();
				}

				nextNode = std::move(currentNode);
				std::cout << "\t\tPoped " << op.asString() << std::endl;//<< " with " <<  nextNode->left->token.asString() << " and " << nextNode->right->token.asString() << std::endl;
			}

			return (currentNode == nullptr  && nextNode == nullptr) ? nullptr : std::move(nextNode);
		}

		 void ska::ShuntingYardExpressionParser::error() {
			throw std::runtime_error("syntax error");
		}



