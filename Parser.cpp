#include <iostream>
#include "Parser.h"
#include "ReservedKeywordsPool.h"
	
		ska::Parser::Parser(std::vector<Token> input) :
			m_input(std::move(input)),
			m_shuntingYardParser(*this, m_input) {
		}

		std::pair<ska::ASTNode, ska::Scope> ska::Parser::parse() {
			auto scope = ska::Scope { nullptr };
			auto ast = ska::ASTNode{};
			m_currentAst = &ast;
			m_currentScope = &scope;

			if (!m_input.empty()) {
				m_lookAhead = &m_input[0];
				statement();
			}
			m_currentAst = nullptr;
			m_currentScope = nullptr;
			return std::make_pair(std::move(ast), std::move(scope));
		}

		void ska::Parser::statement() {
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
				{
					std::cout << "expression found" << std::endl;
					auto expressionResult = expr();
					match(Token{ ";", TokenType::SYMBOL });
					if(expressionResult == nullptr) {
						std::cout << "null" << std::endl;
						m_lookAhead = nullptr;
						return;
					}
				}
				break;
			}
		}

		void ska::Parser::matchRange(const std::string& content) {
			if (content[0] == '{' ) {
				std::cout << "block start detected" << std::endl;
				//branch();
				match(Token{ "{", TokenType::RANGE });
				m_currentScope = &m_currentScope->add();
				do {
					optstatement();
				} while (m_lookAhead != nullptr && (*m_lookAhead) != Token{ "}", TokenType::RANGE });
				match(Token{ "}", TokenType::RANGE });
				//unbranch();
				std::cout << "block end" << std::endl;
				m_currentScope = &m_currentScope->parent();
			} else {
				optexpr(Token{ ";", TokenType::SYMBOL });
			}
		}

		void ska::Parser::matchReservedKeyword(const std::size_t keywordIndex) {
			switch (keywordIndex) {

			case ReservedKeywords::FOR:
				std::cout << "for loop 1st expression" << std::endl;
				match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				//branch();
				match(Token{ "(", TokenType::RANGE });			
				optstatement();
				//unbranch();
				
				std::cout << "2nd for loop expression" << std::endl;
				//branch();
				optexpr(Token{ ";", TokenType::SYMBOL });
				match(Token{ ";", TokenType::SYMBOL });
				//unbranch();
				
				std::cout << "3rd for loop expression" << std::endl;
				//branch();
				optexpr(Token{ ")", TokenType::SYMBOL });
				match(Token{ ")", TokenType::RANGE });
				//unbranch();

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
					std::cout << "variable declaration" << std::endl;
					match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
					const auto& identifier = match(TokenType::IDENTIFIER);
					match(Token{ "=", TokenType::SYMBOL });
					std::cout << "equal sign matched, reading expression" << std::endl;
					auto value = expr();
					match(Token{ ";", TokenType::SYMBOL });
					std::cout << "expression end with symbol ;" << std::endl;
					m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), std::move(value));
				}
				break;

			case ReservedKeywords::FUNCTION: {
					std::cout << "function creation" << std::endl;
					match(Token{ ReservedKeywords::FUNCTION, TokenType::RESERVED });
					match(Token{ "(", TokenType::RANGE });
					
					auto isRightParenthesis = (*m_lookAhead) == Token {")", TokenType::SYMBOL};
					auto isComma = true;
					while (!isRightParenthesis && isComma) {
						if (m_lookAhead->type() != TokenType::SYMBOL) {
							std::cout << "parameter detected, reading identifier" << std::endl;
							match(TokenType::IDENTIFIER);
							isComma = (*m_lookAhead) == Token {",", TokenType::SYMBOL};
							if(isComma) {
								match(Token{ ",", TokenType::SYMBOL });
							}
						}
						isRightParenthesis = (*m_lookAhead) == Token {")", TokenType::SYMBOL};
					}

					match(Token{ ")", TokenType::RANGE });
					std::cout << "reading function statement" << std::endl;
					statement();
				}
				break;

			default:
				error();
			}

		}

		std::unique_ptr<ska::ASTNode> ska::Parser::expr() {
			auto value = m_shuntingYardParser.parse(m_lookAheadIndex);
			m_lookAheadIndex = value.second;
			m_lookAhead = &m_input[m_lookAheadIndex];
			return std::move(value.first);
		}

		void ska::Parser::error() {
			throw std::runtime_error("syntax error");
		}

		const ska::Token& ska::Parser::match(const TokenType type) {
			if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
				const auto& result = *m_lookAhead;
				//pushToken();
				nextToken();
				return result;
			}
			error();
		}

		ska::Token ska::Parser::match(Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				return match(t.type());
			} else {
				error();
				return t;
			}
		}

		void ska::Parser::pushToken() {
			assert(m_lookAhead != nullptr);
			if (m_lookAhead->type() != TokenType::RANGE) {
				//m_currentAst->addChild(*m_lookAhead);
			}
		}

		void ska::Parser::unbranch() {
			//m_currentAst = &m_currentAst->parent;			
		}

		void ska::Parser::branch() {
			assert(m_lookAhead != nullptr);
			if (m_currentAst != nullptr) {
				//auto& newToken = m_currentAst->addOtherChild(Token{});
				//m_currentAst = &newToken;
			}
		}

		void ska::Parser::nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}

		std::unique_ptr<ska::ASTNode> ska::Parser::optexpr(const Token& mustNotBe) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				return expr();
			}
			return nullptr;
		}

		void ska::Parser::optstatement(const Token& mustNotBe) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				statement();
			}
			else {
				match(Token{ ";", TokenType::SYMBOL });
			}
		}
