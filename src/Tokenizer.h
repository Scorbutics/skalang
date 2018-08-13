#pragma once
#include <string>
#include <vector>
#include <bitset>
#include <cctype>
#include <unordered_set>

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
		static std::unordered_set<std::string> BuildAllowedMultipleCharTokenSymbolsSet();

		RequiredToken determineCurrentToken(const std::size_t startIndex) const;
		Token tokenizeNext(const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		RequiredToken initializeCharType(const ska::TokenType charTokenType) const;
		Token finalizeToken(std::size_t index, const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		TokenType calculateCharacterTokenType(const char c) const;

		static bool isWordCharacter(const int c) {
			return std::isalnum(c) || c == '_';
		}
		

		static void push(Token t, std::vector<Token>& symbolStack, std::vector<Token>& output);

		static bool isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack);

		static int stopSymbolCharAggregation(char symbol);
		
		static const std::unordered_set<std::string> ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS;
		const ReservedKeywordsPool& m_reserved;
		std::string m_input;
	};
}
