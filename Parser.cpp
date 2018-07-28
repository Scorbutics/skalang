#include <iostream>
#include "Parser.h"
#include "ReservedKeywordsPool.h"

		ska::Parser::Parser(TokenReader& input) :
			m_input(input),
			m_shuntingYardParser(*this, m_input) {
		}

		std::pair<ska::ASTNode, ska::Scope> ska::Parser::parse() {
			auto scope = ska::Scope { nullptr };
			auto ast = ska::ASTNode{};
			m_currentAst = &ast;
			m_currentScope = &scope;

			statement();
			
			m_currentAst = nullptr;
			m_currentScope = nullptr;
			return std::make_pair(std::move(ast), std::move(scope));
		}

		void ska::Parser::statement() {
			if (m_input.empty()) {
				return;
			}

			std::string* content = nullptr;
			const auto token = m_input.actual();
			switch (token.type()) {
			case TokenType::RESERVED:
				matchReservedKeyword(std::get<std::size_t>(token.content()));
				break;

			case TokenType::RANGE:
				matchRange(std::get<std::string>(token.content()));
				break;

			default:
				{
					std::cout << "expression found" << std::endl;
					auto expressionResult = expr();
					m_input.match(Token{ ";", TokenType::SYMBOL });
					if(expressionResult == nullptr) {
						std::cout << "null expression" << std::endl;
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
				m_input.match(Token{ "{", TokenType::RANGE });
				m_currentScope = &m_currentScope->add();
				do {
					optstatement();
				} while (!m_input.expect(Token{ "}", TokenType::RANGE }));
				m_input.match(Token{ "}", TokenType::RANGE });
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
				std::cout << "1st for loop expression" << std::endl;
				m_input.match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				//branch();
				m_input.match(Token{ "(", TokenType::RANGE });
				optstatement();
				//unbranch();

				std::cout << "2nd for loop expression" << std::endl;
				//branch();
				optexpr(Token{ ";", TokenType::SYMBOL });
				m_input.match(Token{ ";", TokenType::SYMBOL });
				//unbranch();

				std::cout << "3rd for loop expression" << std::endl;
				//branch();
				optexpr(Token{ ")", TokenType::RANGE });
				m_input.match(Token{ ")", TokenType::RANGE });
				//unbranch();

				statement();
				std::cout << "end for loop statement" << std::endl;
				break;

			case ReservedKeywords::ELSE:
				m_input.match(Token{ ReservedKeywords::ELSE, TokenType::RESERVED });
				statement();
				break;
			case ReservedKeywords::IF:
				m_input.match(Token{ ReservedKeywords::IF, TokenType::RESERVED });
				break;

			case ReservedKeywords::VAR:
				{
					std::cout << "variable declaration" << std::endl;
					m_input.match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
					const auto& identifier = m_input.match(TokenType::IDENTIFIER);
					m_input.match(Token{ "=", TokenType::SYMBOL });
					std::cout << "equal sign matched, reading expression" << std::endl;
					auto value = expr();
					m_input.match(Token{ ";", TokenType::SYMBOL });
					std::cout << "expression end with symbol ;" << std::endl;
					m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), std::move(value));
				}
				break;

			case ReservedKeywords::FUNCTION: {
					matchFunction();
				}
				break;

			default:
				error();
			}

		}

		std::unique_ptr<ska::ASTNode> ska::Parser::expr(const Token& token) {
			return m_shuntingYardParser.parse(token);
		}

		void ska::Parser::error() {
			throw std::runtime_error("syntax error");
		}

		void ska::Parser::matchFunction() {
			std::cout << "function creation" << std::endl;
			m_input.match(Token{ ReservedKeywords::FUNCTION, TokenType::RESERVED });
			m_input.match(Token{ "(", TokenType::RANGE });

			auto isRightParenthesis = m_input.expect(Token{ ")", TokenType::SYMBOL });
			auto isComma = true;
			while (!isRightParenthesis && isComma) {
				if (!m_input.expect(TokenType::SYMBOL)) {
					std::cout << "parameter detected, reading identifier" << std::endl;
					m_input.match(TokenType::IDENTIFIER);
					isComma = m_input.expect(Token{ ",", TokenType::SYMBOL });
					if (isComma) {
						m_input.match(Token{ ",", TokenType::SYMBOL });
					}
				}
				isRightParenthesis = m_input.expect(Token{ ")", TokenType::SYMBOL });
			}

			m_input.match(Token{ ")", TokenType::RANGE });
			std::cout << "reading function statement" << std::endl;
			statement();
			std::cout << "function read." << std::endl;
		}

		void ska::Parser::unbranch() {
			//m_currentAst = &m_currentAst->parent;
		}

		void ska::Parser::branch() {
			//assert(m_lookAhead != nullptr);
			if (m_currentAst != nullptr) {
				//auto& newToken = m_currentAst->addOtherChild(Token{});
				//m_currentAst = &newToken;
			}
		}

		std::unique_ptr<ska::ASTNode> ska::Parser::optexpr(const Token& mustNotBe) {
			if (!m_input.expect(mustNotBe)) {
				return expr(mustNotBe);
			}
			return nullptr;
		}

		void ska::Parser::optstatement(const Token& mustNotBe) {
			if (!m_input.expect(mustNotBe)) {
				statement();
			} else {
				m_input.match(Token{ ";", TokenType::SYMBOL });
			}
		}
