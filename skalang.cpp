#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "Tokenizer.h"
#include "Scope.h"
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

		Scope parse() {
			auto scope = ska::Scope { nullptr };
			if (!m_input.empty()) {
				auto* currentScope = &scope;
				m_lookAhead = &m_input[0];
				statement(currentScope);
				
			}
			return scope;
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
				content = &std::get<std::string>(m_lookAhead->content);
				if (*content == "{") {
					currentScope = &currentScope->add();
				} else if (*content == "}") {
					currentScope = &currentScope->parent();
				}
				break;
			
			default:
				currentScope->addToken(*m_lookAhead);
				break;
			}
		}

		void matchReservedKeyword(Scope& scope, const std::size_t keywordIndex) {
			switch (keywordIndex) {
			case ReservedKeywords::FOR:
				match(scope, Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				match(scope, Token{ "(", TokenType::RANGE });
				optexpr(scope);
				match(scope, Token{ ";", TokenType::SYMBOL });
				optexpr(scope);
				match(scope, Token{ ";", TokenType::SYMBOL });
				optexpr(scope);
				match(scope, Token{ ")", TokenType::RANGE });
				statement(&scope);
				break;
				
			case ReservedKeywords::IF:
				break;
			case ReservedKeywords::VAR:
				match(scope, Token{ ReservedKeywords::VAR, TokenType::RESERVED });
				expr(scope);
				match(scope, Token{ "=", TokenType::SYMBOL });
				optexpr(scope);
				match(scope, Token{ ";", TokenType::SYMBOL });
				statement(&scope);
				break;
			default:
				throw std::runtime_error("syntax error");
				break;
			}

		}
		
		void optexpr(Scope& scope) {
			if (m_lookAhead != nullptr && m_lookAhead->type != TokenType::EMPTY) {
				expr(scope);
			}
		}

		void expr(Scope& scope) {
			//match(scope, *m_lookAhead);
		}

		void match(Scope& scope, Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				m_lookAhead = m_lookAheadIndex  < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
			} else {
				throw std::runtime_error("syntax error");
			}
		}

		std::vector<Token> m_input;
		std::size_t m_lookAheadIndex = 0;
		Token* m_lookAhead = nullptr;
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

