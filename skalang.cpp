#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

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

	class Parser {
	public:
		Parser(std::vector<Token> input) :
			m_input(std::move(input)) {
		}

		std::pair<ASTNode, Scope> parse() {
			auto scope = ska::Scope { nullptr };
			auto ast = ska::ASTNode{};
			m_currentAst = &ast;

			if (!m_input.empty()) {
				auto* currentScope = &scope;
				m_lookAhead = &m_input[0];
				statement(currentScope);
				
			}
			m_currentAst = nullptr;
			return std::make_pair(std::move(ast), std::move(scope));
		}

	private:
		void statement(Scope* currentScope) {
			assert(currentScope != nullptr);
			if (m_lookAhead == nullptr) {
				return;
			}

			std::string* content = nullptr;
			switch (m_lookAhead->type) {
			case TokenType::RESERVED:
				matchReservedKeyword(*currentScope, std::get<std::size_t>(m_lookAhead->content));
				break;

			case TokenType::RANGE:
				matchRange(*currentScope, std::get<std::string>(m_lookAhead->content));
				break;
			
			default:
				expr(*currentScope);
				
				break;
			}
		}

		void matchRange(Scope& scope, const std::string& content) {
			if (content[0] == '{' ) {
				
				branch();
				match(scope, Token{ "{", TokenType::RANGE });
				scope = &scope.add();
				do {
					optstatement(scope);
				} while (m_lookAhead != nullptr && (*m_lookAhead) != Token{ "}", TokenType::RANGE });
				match(scope, Token{ "}", TokenType::RANGE });
				unbranch();

				scope = &scope.parent();
			} else {
				nextToken();
			}
		}

		void matchReservedKeyword(Scope& scope, const std::size_t keywordIndex) {
			switch (keywordIndex) {

			case ReservedKeywords::FOR:
				match(scope, Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				branch();
				match(scope, Token{ "(", TokenType::RANGE });			
				optstatement(scope);
				unbranch();
				
				branch();
				optexpr(scope, Token{ ";", TokenType::SYMBOL });
				match(scope, Token{ ";", TokenType::SYMBOL });
				unbranch();
				
				branch();
				optexpr(scope, Token{ ")", TokenType::SYMBOL });
				match(scope, Token{ ")", TokenType::RANGE });
				unbranch();

				statement(&scope);
				break;
				
			case ReservedKeywords::ELSE:
				match(scope, Token{ ReservedKeywords::ELSE, TokenType::RESERVED });
				break;
			case ReservedKeywords::IF:
				match(scope, Token{ ReservedKeywords::IF, TokenType::RESERVED });
				break;

			case ReservedKeywords::VAR:
				match(scope, Token{ ReservedKeywords::VAR, TokenType::RESERVED });
				{
					const auto& identifier = matchUnknown(scope, TokenType::IDENTIFIER);
					match(scope, Token{ "=", TokenType::SYMBOL });
					const auto& value = expr(scope);
					match(scope, Token{ ";", TokenType::SYMBOL });
					scope.registerIdentifier(std::get<std::string>(identifier.content), value);
				}
				break;

			case ReservedKeywords::FUNCTION:
				match(scope, Token{ ReservedKeywords::FUNCTION, TokenType::RESERVED });
				break;

			default:
				throw std::runtime_error("syntax error");
			}

		}
		
		const Token* optexpr(Scope& scope, const Token& mustNotBe = Token{}) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				return &expr(scope);
			}
			return nullptr;
		}

		void optstatement(Scope& scope, const Token& mustNotBe = Token{ }) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				statement(&scope);
			} else {
				match(scope, Token{ ";", TokenType::SYMBOL });
			}
		}

		const Token& expr(Scope& scope) {
			const auto& result = *m_lookAhead;
			const auto& value = std::get<std::string>(m_lookAhead->content);
			
			switch (m_lookAhead->type) {
			case TokenType::IDENTIFIER:
				matchUnknown(scope, TokenType::IDENTIFIER);
				break;
			
			case TokenType::DIGIT:
				matchUnknown(scope, TokenType::DIGIT);
				break;
			
			case TokenType::STRING:
				matchUnknown(scope, TokenType::STRING);
				break;
			
			case TokenType::RANGE:
				//pushToken(result);
				switch (value[0]) {
				case '(' :
					break;
				case ')':
					break;
				default:
					throw std::runtime_error("syntax error");
				}
				nextToken();
				break;

			case TokenType::SYMBOL:
				switch (value[0]) {
				case '=':
					match(scope, Token{ "=", TokenType::SYMBOL });
					expr(scope);
					break;
				
				default:
					matchUnknown(scope, TokenType::SYMBOL);
					break;
				}
				break;

			default:
				throw std::runtime_error("syntax error");
			}
			return result;
		}

		const Token& matchUnknown(Scope& scope, const TokenType type) {
			if (m_lookAhead != nullptr && m_lookAhead->type == type) {
				const auto& result = *m_lookAhead;
				pushToken();
				nextToken();
				return result;
			}
			throw std::runtime_error("syntax error");
		}

		void match(Scope& scope, Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				pushToken();
				nextToken();
			} else {
				throw std::runtime_error("syntax error");
			}
		}

		void pushToken() {
			assert(m_lookAhead != nullptr);
			if (m_lookAhead->type != TokenType::RANGE) {
				if (m_currentAst->empty()) {
					m_currentAst->token = *m_lookAhead;
				} else {
					m_currentAst->addChild(*m_lookAhead);
				}
			}
		}

		void unbranch() {
			m_currentAst = &m_currentAst->parent();			
		}

		void branch() {
			assert(m_lookAhead != nullptr);
			if (m_currentAst != nullptr) {
				auto& newToken = m_currentAst->addChild(Token{});
				m_currentAst = &newToken;
			}
		}

		void nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}

		std::vector<Token> m_input;
		std::size_t m_lookAheadIndex = 0;
		Token* m_lookAhead = nullptr;
		ASTNode* m_currentAst;
	};
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
			std::cout << "[" << std::setw(10) << TokenTypeSTR[static_cast<std::size_t>(token.type)] << "]\t" << (token.type == ska::TokenType::RESERVED ? "" : std::get<std::string>(token.content)) << std::endl;
		}

		auto parser = ska::Parser { std::move(tokens) };
		auto tokenTree = parser.parse();

		std::cout << std::endl;
	}
    return 0;
}

