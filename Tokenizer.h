#pragma once
#include <string>
#include <vector>
#include <bitset>
#include <cctype>

#include "ReservedKeywordsPool.h"

namespace ska {

	enum class TokenType {
		RESERVED,
		IDENTIFIER,
		DIGIT,
		SPACE,
		STRING,
		RANGE,
		SYMBOL,
		EMPTY,
		UNUSED_LAST_Length
	};

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

	struct Token {
		std::string content;
		TokenType type;
	};

	struct RequiredToken {
		ska::TokenType current = ska::TokenType::EMPTY;
		bool required = true;
		std::bitset<static_cast<std::size_t>(TokenType::UNUSED_LAST_Length)> requiredOrUntil;
	};

	class Tokenizer {
	public:
		Tokenizer(const ReservedKeywordsPool& reserved, std::string input);
		std::vector<Token> tokenize() const;

	private:
		RequiredToken determineCurrentToken(const std::size_t startIndex) const;
		Token tokenizeNext(const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		RequiredToken initializeCharType(const ska::TokenType charTokenType) const;
		Token finalizeToken(std::size_t index, const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		TokenType calculateCharacterTokenType(const char c) const;

		static bool isWordCharacter(const int c) {
			return std::isalnum(c) || c == '_';
		}

		std::string m_input;
		const ReservedKeywordsPool& m_reserved;
	};
}