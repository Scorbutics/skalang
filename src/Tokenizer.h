#pragma once
#include <string>
#include <vector>
#include <bitset>
#include <cctype>

#include "ReservedKeywordsPool.h"
#include "Token.h"

namespace ska {

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

		const ReservedKeywordsPool& m_reserved;
		std::string m_input;
	};
}
