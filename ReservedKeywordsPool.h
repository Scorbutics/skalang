#pragma once

#include <unordered_map>

#include "Token.h"

namespace ska {

	enum class TokenGrammar {
		FOR,
		IF,
		ELSE,
		VARIABLE,
		FUNCTION,
		RETURN,
		AFFECTATION,
		BLOCK_BEGIN,
		BLOCK_END,
		PARENTHESIS_BEGIN,
		PARENTHESIS_END,
		STRING_DELIMITER,
		STATEMENT_END,
		ARGUMENT_DELIMITER,
		UNUSED_LAST_Length
	};


	struct ReservedKeywordsPool {
		struct TokenInfo {
			Token token;
		};

		ReservedKeywordsPool() : 
			pool(BuildPool()),
			patterns(BuildPatterns(pool)){
		}
		
		const std::unordered_map<std::string, TokenInfo> pool;
		
		template <TokenGrammar token>
		const Token& pattern() const {
			return patterns[static_cast<std::size_t>(token)].token;
		}

		const Token& pattern(const std::size_t token) const {
			return patterns[token].token;
		}
		
	private:
		const std::vector<TokenInfo> patterns;

		static std::vector<TokenInfo> BuildPatterns(const std::unordered_map<std::string, TokenInfo>& pool) {
			auto patterns = std::vector<TokenInfo>(pool.size());

			for (const auto& p : pool) {
				patterns.emplace_back(p.second);
			}

			return patterns;
		}

		static std::unordered_map<std::string, TokenInfo> BuildPool() {
			auto pool = std::unordered_map<std::string, TokenInfo>{};

			pool.emplace("for", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::FOR), TokenType::RESERVED } });
			pool.emplace("if", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::IF), TokenType::RESERVED } });
			pool.emplace("else", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::ELSE), TokenType::RESERVED } });
			pool.emplace("var", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::VARIABLE), TokenType::RESERVED } });
			pool.emplace("function", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::FUNCTION), TokenType::RESERVED } });
			pool.emplace("return", TokenInfo{ Token{ static_cast<std::size_t>(TokenGrammar::RETURN), TokenType::RESERVED } });

			pool.emplace("{", TokenInfo{ Token{ "{", TokenType::RANGE } });
			pool.emplace("}", TokenInfo{ Token{ "}", TokenType::RANGE } });
			pool.emplace("(", TokenInfo{ Token{ "(", TokenType::RANGE }});
			pool.emplace(")", TokenInfo{ Token{ ")", TokenType::RANGE } });
			pool.emplace("\"", TokenInfo{ Token{ "\"", TokenType::SYMBOL } });
			pool.emplace(";", TokenInfo{ Token{ ";", TokenType::SYMBOL } });
			pool.emplace(",", TokenInfo{ Token{ ",", TokenType::SYMBOL } });

			
			return pool;
		}
	};
}