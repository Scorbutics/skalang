#pragma once

#include <unordered_map>
#include <iostream>
#include "Token.h"

namespace ska {

	enum class TokenGrammar {
		FOR,
		IF,
		ELSE,
		VARIABLE,
		FUNCTION,
		RETURN,
		METHOD_CALL_OPERATOR,
		AFFECTATION,
		BLOCK_BEGIN,
		BLOCK_END,
		PARENTHESIS_BEGIN,
		PARENTHESIS_END,
		STRING_DELIMITER,
		STATEMENT_END,
		ARGUMENT_DELIMITER
	};


	struct ReservedKeywordsPool {
		struct TokenInfo {
		    std::string patternString;
			Token token;
		};

		ReservedKeywordsPool() :
			pool(BuildPool()),
			patterns(BuildPatterns(pool)){
		}

		const std::unordered_map<std::string, TokenInfo> pool {};

		template <TokenGrammar token>
		const Token& pattern() const {
			return patterns[static_cast<std::size_t>(token)].token;
		}

		const Token& pattern(const std::size_t token) const {
			return patterns[token].token;
		}

        	const std::string& patternString(const std::size_t token) const {
			return patterns[token].patternString;
		}

	private:
		const std::vector<TokenInfo> patterns;

		static std::vector<TokenInfo> BuildPatterns(const std::unordered_map<std::string, TokenInfo>& pool) {
			auto patterns = std::vector<TokenInfo>();
			patterns.resize(pool.size());

			auto index = 0u;
			for (const auto& p : pool) {
                		if(p.second.token.type() == TokenType::RESERVED) {
                    			patterns[std::get<std::size_t>(p.second.token.content())] = p.second;
                		}
			}

            		patterns[static_cast<std::size_t>(ska::TokenGrammar::AFFECTATION)]           	= pool.at("=");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::BLOCK_BEGIN)]           	= pool.at("{");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::BLOCK_END)]             	= pool.at("}");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::PARENTHESIS_BEGIN)]     	= pool.at("(");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::PARENTHESIS_END)]       	= pool.at(")");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::STRING_DELIMITER)]      	= pool.at("\"");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::STATEMENT_END)]         	= pool.at(";");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::ARGUMENT_DELIMITER)]    	= pool.at(",");
			patterns[static_cast<std::size_t>(ska::TokenGrammar::METHOD_CALL_OPERATOR)]	= pool.at(".");
			
			return patterns;
		}

		static std::unordered_map<std::string, TokenInfo> BuildPool() {
			auto pool = std::unordered_map<std::string, TokenInfo>{};

			pool.emplace("for", TokenInfo{ "for", Token{ static_cast<std::size_t>(TokenGrammar::FOR), TokenType::RESERVED } });
			pool.emplace("if", TokenInfo{ "if", Token{ static_cast<std::size_t>(TokenGrammar::IF), TokenType::RESERVED } });
			pool.emplace("else", TokenInfo{ "else", Token{ static_cast<std::size_t>(TokenGrammar::ELSE), TokenType::RESERVED } });
			pool.emplace("var", TokenInfo{ "var", Token{ static_cast<std::size_t>(TokenGrammar::VARIABLE), TokenType::RESERVED } });
			pool.emplace("function", TokenInfo{ "function", Token{ static_cast<std::size_t>(TokenGrammar::FUNCTION), TokenType::RESERVED } });
			pool.emplace("return", TokenInfo{ "return", Token{ static_cast<std::size_t>(TokenGrammar::RETURN), TokenType::RESERVED } });

			pool.emplace(".", TokenInfo{ ".", Token{ ".", TokenType::DOT_SYMBOL } });
			pool.emplace("=", TokenInfo{ "=", Token{ "=", TokenType::SYMBOL } });
			pool.emplace("{", TokenInfo{ "{", Token{ "{", TokenType::RANGE } });
			pool.emplace("}", TokenInfo{ "}", Token{ "}", TokenType::RANGE } });
			pool.emplace("(", TokenInfo{ "(", Token{ "(", TokenType::RANGE }});
			pool.emplace(")", TokenInfo{ ")", Token{ ")", TokenType::RANGE } });
			pool.emplace("\"", TokenInfo{ "\"", Token{ "\"", TokenType::SYMBOL } });
			pool.emplace(";", TokenInfo{ ";", Token{ ";", TokenType::SYMBOL } });
			pool.emplace(",", TokenInfo{ ",", Token{ ",", TokenType::SYMBOL } });

			return pool;
		}
	};
}
