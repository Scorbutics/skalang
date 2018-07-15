#include "ShuntingYardExpressionParser.h"
#include "AST.h"


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
		
		ska::ShuntingYardExpressionParser::ShuntingYardExpressionParser(const std::vector<Token>& input) : 
			m_input(input) {
		}
		
		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::parse(const std::size_t indexStart) {
			m_lookAheadIndex = indexStart;
			m_lookAhead = &m_input[m_lookAheadIndex];
			return expression();
		}
		
		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::expression() {
			auto node = ASTNode{};
			
			std::cout << "Shunting Yards steps :" << std::endl;
			
			while(m_lookAhead != nullptr && m_lookAhead->type() != TokenType::EMPTY && (*m_lookAhead) != Token {";", TokenType::SYMBOL}) {
				const auto& result = *m_lookAhead;
				const auto& value = std::get<std::string>(result.content());
				
				PrintPtr(m_operands, "Operands");
				Print(m_operators, "Operators");
				
				switch (result.type()) {
				case TokenType::STRING:
				case TokenType::RESERVED:
				case TokenType::IDENTIFIER:
				case TokenType::DIGIT:
					std::cout << "Pushing operand digit " << value << std::endl;
					m_operands.push(std::make_unique<ASTNode>(match(result.type())));
					break;

				case TokenType::RANGE:
					switch (value[0]) {
					case '(' :
						std::cout << "Range begin" << std::endl;
						m_operators.emplace(match(Token{ "(", TokenType::RANGE }));
						break;
					case ')': {
							m_operators.emplace(match(Token{ ")", TokenType::RANGE }));
							std::cout << "Range end" << std::endl;
							auto rangeOperandResult = popUntil([](const Token& t) {
								const auto& strValue = t.asString();
								if(t.type() == TokenType::RANGE) {
									return (strValue.empty() || strValue[0] == '(' )  ? -1 : 1;
								}
								std::cout << "\tPoping " << strValue << std::endl;
								return 0;
							});
							assert(rangeOperandResult != nullptr);
							m_operands.push(std::move(rangeOperandResult));
							m_operators.pop();
						}
						break;
						
					default:
						error();
					}
					break;

				case TokenType::SYMBOL: {
					std::cout << "Pushing operator symbol " << value << std::endl;
						const auto shouldPopOperatorsStack = checkLessPriorityToken(value[0]);
						auto operatorTop = m_operators.empty() ? Token{} : m_operators.top();
						if(shouldPopOperatorsStack) {
							std::cout << "Less precedence, poping " << operatorTop.asString() <<  " before adding " << value << std::endl;
							auto poped = 0u;
							auto popedToken = popUntil([&](const Token& t) {
								if(poped >= 1) {
									return -1;
								}
								poped++;
								return 0;
							});
							assert(popedToken != nullptr);
							m_operands.push(std::move(popedToken));
						}
						m_operators.emplace(match(TokenType::SYMBOL));
					}
					break;

				default:
					error();
				}
			
			}
			std::cout << "Poping everything" << std::endl;
			return popUntil([](const auto& t) {
				return 0;
			});
		}

		bool ska::ShuntingYardExpressionParser::checkLessPriorityToken(const char tokenChar) const {
			const auto& topOperatorContent = (m_operators.empty() || m_operators.top().asString().empty()) ? '\0' : m_operators.top().asString()[0];
			return PRIORITY_MAP.find(tokenChar) != PRIORITY_MAP.end() &&
					PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
					PRIORITY_MAP.at(tokenChar) < PRIORITY_MAP.at(topOperatorContent);
		}

		std::unique_ptr<ska::ASTNode> ska::ShuntingYardExpressionParser::popUntil(PopPredicate predicate) {
			auto currentNode = std::unique_ptr<ASTNode> {};
			auto nextNode = std::unique_ptr<ASTNode> {};
			
			while(true) {
				if(m_operators.empty() || m_operands.empty()) {
					break;
				}
				
				const auto op = m_operators.top(); 
				const auto analyzeToken = predicate(op);
				//Flow control loop predicate by token
				if(analyzeToken < 0) {
					break;
				} else if (analyzeToken > 0) {
					m_operators.pop();
					continue;
				}
				m_operators.pop();
				
				currentNode = std::make_unique<ASTNode>();
				currentNode->token = op;
				currentNode->right = std::move(m_operands.top()); m_operands.pop();
				
				if(nextNode != nullptr) {
					currentNode->left = std::move(nextNode);
				}else {
					currentNode->left = std::move(m_operands.top()); m_operands.pop();
				}
				
				nextNode = std::move(currentNode);
				std::cout << "\tPoped " << op.asString() << std::endl;//<< " with " <<  nextNode->left->token.asString() << " and " << nextNode->right->token.asString() << std::endl; 
			}
			
			return (currentNode == nullptr  && nextNode == nullptr) ? nullptr : std::move(nextNode);
		}

		 void ska::ShuntingYardExpressionParser::error() {
			throw std::runtime_error("syntax error");
		}
		
	
		const ska::Token& ska::ShuntingYardExpressionParser::match(const TokenType type) {
			if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
				const auto& result = *m_lookAhead;
				nextToken();
				if(m_lookAhead != nullptr) {
					//std::cout << "Next token " << m_lookAhead->asString() << std::endl;
				}
				return result;
			}
			error();
		}

		ska::Token ska::ShuntingYardExpressionParser::match(Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				return match(t.type());
			} else {
				error();
				return t;
			}
		}

		void ska::ShuntingYardExpressionParser::nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}
	

