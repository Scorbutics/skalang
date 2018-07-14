#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <cassert>
#include <stack>
#include <functional>

#include "Tokenizer.h"
#include "Scope.h"
#include "AST.h"
#include "ReservedKeywordsPool.h"

constexpr const char* TokenTypeSTR[] = {
	"RESERVED",
	"IDENTIFIER",
	"DIGIT",
	"SPACE",
	"STRING",
	"RANGE",
	"SYMBOL",
	"EMPTY",
	"UNUSED_LAST_Length"
};

namespace ska {

	class ShuntingYardExpressionParser {
		using PopPredicate = std::function<int(const Token&)>;
		
		static std::unordered_map<char, int> BuildPriorityMap() {
			auto result = std::unordered_map<char, int>{};
			
			result.emplace('+', 100);
			result.emplace('-', 100);
			result.emplace('*', 200);
			result.emplace('/', 200);
			
			return result;
		}
		
		static std::unordered_map<char, int> PRIORITY_MAP;
		
	public:
		ShuntingYardExpressionParser(std::vector<Token> input) : 
			m_input(std::move(input)) {
		}
		
		std::unique_ptr<ASTNode> parse() {
			m_lookAhead = &m_input[m_lookAheadIndex];
			return expression();
		}
		
	private:
		std::unique_ptr<ASTNode> expression() {
			auto node = ASTNode{};
			
			std::cout << "Shunting Yards steps :" << std::endl;
			
			while(m_lookAhead != nullptr && m_lookAhead->type() != TokenType::EMPTY) {
				const auto& result = *m_lookAhead;
				const auto& value = std::get<std::string>(result.content());
				
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
					case ')':
						m_operators.emplace(match(Token{ ")", TokenType::RANGE }));
						std::cout << "Range end" << std::endl;
						m_operands.push(popUntil([](const Token& t) {
							const auto& strValue = t.asString();
							if(t.type() == TokenType::RANGE) {
								//std::cout << "\tRange " << strValue << std::endl;
								return (strValue.empty() || strValue[0] == '(' )  ? -1 : 1;
							}
							std::cout << "\tPoping " << strValue << std::endl;
							return 0;
						}));
						std::cout << m_operators.top().asString() << std::endl;
						break;
					default:
						error();
					}
					break;

				case TokenType::SYMBOL: {
					std::cout << "Pushing operator symbol " << value << std::endl;
						const auto shouldPopOperatorsStack = checkLessPriorityToken(value[0]);
						auto operatorTop = m_operators.empty() ? Token{} : m_operators.top();
						m_operators.emplace(match(TokenType::SYMBOL));
						if(shouldPopOperatorsStack) {
							std::cout << "Less precedence, poping " << value << " and " << operatorTop.asString() << std::endl;
							auto poped = 0u;
							auto popedToken = popUntil([&](const Token& t) {
								if(poped >= 2) {
									return -1;
								}
								poped++;
								return 0;
							});
							if(popedToken != nullptr) {
								m_operands.push(std::move(popedToken));
							}
						}
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

		bool checkLessPriorityToken(const char tokenChar) const {
			const auto& topOperatorContent = (m_operators.empty() || m_operators.top().asString().empty()) ? '\0' : m_operators.top().asString()[0];
			return PRIORITY_MAP.find(tokenChar) != PRIORITY_MAP.end() &&
					PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
					PRIORITY_MAP.at(tokenChar) < PRIORITY_MAP.at(topOperatorContent);
		}

		std::unique_ptr<ASTNode> popUntil(PopPredicate predicate) {
			auto currentNode = std::unique_ptr<ASTNode> {};
			auto nextNode = std::unique_ptr<ASTNode> {};
			
			while(true) {
				if(m_operators.empty() || m_operands.empty()) {
					break;
				}
				
				const auto op = m_operators.top(); m_operators.pop();
				const auto analyzeToken = predicate(op);
				//Flow control loop predicate by token
				if(analyzeToken < 0) {
					break;
				} else if (analyzeToken > 0) {
					continue;
				}
				
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
			
			return currentNode == nullptr ? nullptr : std::move(nextNode);
		}

		static void error() {
			throw std::runtime_error("syntax error");
		}
		
	
		const Token& match(const TokenType type) {
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

		Token match(Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				return match(t.type());
			} else {
				error();
				return t;
			}
		}

		void nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}
	
		Token* m_lookAhead {};
		std::size_t m_lookAheadIndex = 0;
		std::vector<Token> m_input;
		std::stack<Token> m_operators;
		std::stack<std::unique_ptr<ASTNode>> m_operands;
	};
	
	std::unordered_map<char, int> ShuntingYardExpressionParser::PRIORITY_MAP = BuildPriorityMap();
	
	
	class Parser {
	public:
		Parser(std::vector<Token> input) :
			m_input(std::move(input)) {
		}

		std::pair<ASTNode, Scope> parse() {
			auto scope = ska::Scope { nullptr };
			auto ast = ska::ASTNode{};
			m_currentAst = &ast;
			m_currentScope = &scope;

			if (!m_input.empty()) {
				m_lookAhead = &m_input[0];
				expr();
			}
			m_currentAst = nullptr;
			m_currentScope = nullptr;
			return std::make_pair(std::move(ast), std::move(scope));
		}

	private:
		void statement() {
			if (m_lookAhead == nullptr) {
				return;
			}

			std::string* content = nullptr;
			switch (m_lookAhead->type()) {
			case TokenType::RESERVED:
				matchReservedKeyword(std::get<std::size_t>(m_lookAhead->content()));
				break;

			case TokenType::RANGE:
				matchRange(std::get<std::string>(m_lookAhead->content()));
				break;
			
			default:
				expr();
				break;
			}
		}

		void matchRange(const std::string& content) {
			if (content[0] == '{' ) {
				
				branch();
				match(Token{ "{", TokenType::RANGE });
				m_currentScope = &m_currentScope->add();
				do {
					optstatement();
				} while (m_lookAhead != nullptr && (*m_lookAhead) != Token{ "}", TokenType::RANGE });
				match(Token{ "}", TokenType::RANGE });
				unbranch();

				m_currentScope = &m_currentScope->parent();
			} else {
				optexpr(Token{ ";", TokenType::SYMBOL });
			}
		}

		void matchReservedKeyword(const std::size_t keywordIndex) {
			switch (keywordIndex) {

			case ReservedKeywords::FOR:
				match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				branch();
				match(Token{ "(", TokenType::RANGE });			
				optstatement();
				unbranch();
				
				branch();
				optexpr(Token{ ";", TokenType::SYMBOL });
				match(Token{ ";", TokenType::SYMBOL });
				unbranch();
				
				branch();
				optexpr(Token{ ")", TokenType::SYMBOL });
				match(Token{ ")", TokenType::RANGE });
				unbranch();

				statement();
				break;
				
			case ReservedKeywords::ELSE:
				match(Token{ ReservedKeywords::ELSE, TokenType::RESERVED });
				statement();
				break;
			case ReservedKeywords::IF:
				match(Token{ ReservedKeywords::IF, TokenType::RESERVED });
				break;

			case ReservedKeywords::VAR:
				{
					match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
					const auto& identifier = match(TokenType::IDENTIFIER);
					match(Token{ "=", TokenType::SYMBOL });
					const auto& value = expr();
					match(Token{ ";", TokenType::SYMBOL });
					m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), value);
				}
				break;

			case ReservedKeywords::FUNCTION: {
					match(Token{ ReservedKeywords::FUNCTION, TokenType::RESERVED });
					match(Token{ "(", TokenType::RANGE });

					auto isRightParenthesis = m_lookAhead->type() == TokenType::SYMBOL && std::get<std::string>(m_lookAhead->content()) == ")";
					while (!isRightParenthesis) {
						if (m_lookAhead->type() == TokenType::SYMBOL) {
							expr();
							match(Token{ ",", TokenType::SYMBOL });
						}
						isRightParenthesis = m_lookAhead->type() == TokenType::SYMBOL && std::get<std::string>(m_lookAhead->content()) == ")";
					}

					match(Token{ ")", TokenType::RANGE });
				}
				break;

			default:
				error();
			}

		}

		const Token& expr() {
			auto nodeStack = std::deque<ASTNode>{};
			auto node = ASTNode{};
			const auto& result = *m_lookAhead;
			const auto& value = std::get<std::string>(m_lookAhead->content());
			
			switch (m_lookAhead->type()) {
			case TokenType::IDENTIFIER:
				nodeStack.emplace_back(match(TokenType::IDENTIFIER));
				optexpr(Token{ ";", TokenType::SYMBOL });
				break;
			
			case TokenType::DIGIT:
				nodeStack.emplace_back(match(TokenType::DIGIT));
				optexpr(Token{ ";", TokenType::SYMBOL });
				break;
			
			case TokenType::STRING:
				nodeStack.emplace_back(match(TokenType::STRING));
				break;
			
			case TokenType::RESERVED:
				matchReservedKeyword(ReservedKeywords::FUNCTION);
				break;

			case TokenType::RANGE:
				switch (value[0]) {
				case '(' :
					nodeStack.emplace_back(match(Token{ "(", TokenType::RANGE }));
					expr();
					break;
				case ')':
					nodeStack.emplace_back(match(Token{ ")", TokenType::RANGE }));
					optexpr(Token{ ";", TokenType::SYMBOL });
					break;
				default:
					error();
				}
				break;

			case TokenType::SYMBOL:
				switch (value[0]) {
				case '=':
					nodeStack.emplace_back(match(Token{ "=", TokenType::SYMBOL }));
					expr();
					break;
				
				default:
					nodeStack.emplace_back(match(TokenType::SYMBOL));
					expr();
					break;
				}
				break;

			default:
				error();
			}
			return result;
		}

		static void error() {
			throw std::runtime_error("syntax error");
		}

		const Token& match(const TokenType type) {
			if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
				const auto& result = *m_lookAhead;
				pushToken();
				nextToken();
				return result;
			}
			error();
		}

		Token match(Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				return match(t.type());
			} else {
				error();
				return t;
			}
		}

		void pushToken() {
			assert(m_lookAhead != nullptr);
			if (m_lookAhead->type() != TokenType::RANGE) {
				//m_currentAst->addChild(*m_lookAhead);
			}
		}

		void unbranch() {
			//m_currentAst = &m_currentAst->parent;			
		}

		void branch() {
			assert(m_lookAhead != nullptr);
			if (m_currentAst != nullptr) {
				//auto& newToken = m_currentAst->addOtherChild(Token{});
				//m_currentAst = &newToken;
			}
		}

		void nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}

		const Token* optexpr(const Token& mustNotBe = Token{}) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				return &expr();
			}
			return nullptr;
		}

		void optstatement(const Token& mustNotBe = Token{}) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				statement();
			}
			else {
				match(Token{ ";", TokenType::SYMBOL });
			}
		}

		std::vector<Token> m_input;
		std::size_t m_lookAheadIndex = 0;
		Token* m_lookAhead = nullptr;
		
		ASTNode* m_currentAst;
		Scope* m_currentScope;
	};
}

void PrintTokenTree(const std::unique_ptr<ska::ASTNode>& node) {	
	
	std::cout << node->token.asString() << " | ";

	
	if(node->left != nullptr) {
		PrintTokenTree(node->left);
	}
	
	if(node->right != nullptr) {
		PrintTokenTree(node->right);
	}
	
}

int main() {
	for (;;) {
		auto stop = false;
		std::stringstream ss;

		while (!stop) {
			auto line = std::string{};
			std::getline(std::cin, line);
			stop = line.empty();
			ss << line;
		}

		const auto reservedKeywords = ska::ReservedKeywordsPool{};

		auto tokenizer = ska::Tokenizer { reservedKeywords, ss.str() };
		auto tokens = tokenizer.tokenize();
		for (const auto& token : tokens) {
			std::cout << "[" << std::setw(10) << TokenTypeSTR[static_cast<std::size_t>(token.type())] << "]\t" << (token.type() == ska::TokenType::RESERVED ? "" : std::get<std::string>(token.content())) << std::endl;
		}

		auto parser = ska::ShuntingYardExpressionParser { std::move(tokens) };
		auto tokenTree = parser.parse();
		
		PrintTokenTree(tokenTree);
		std::cout << std::endl;
		break;
	}
    return 0;
}

