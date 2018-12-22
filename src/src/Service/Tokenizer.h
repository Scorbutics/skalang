#pragma once
#include <string>
#include <vector>
#include <bitset>
#include <cctype>
#include <unordered_set>

#include "ReservedKeywordsPool.h"
#include "NodeValue/Token.h"

namespace ska {

	struct RequiredToken {
		ska::TokenType current = ska::TokenType::EMPTY;
		bool required = true;
		std::bitset<static_cast<std::size_t>(TokenType::UNUSED_LAST_Length)> requiredOrUntil;
	};

	enum class SymbolFinalizeTokenState {
		READY,
		FINALIZED_WITH_NEXT_SYMBOL,
		FINALIZED_NO_NEXT_SYMBOL
	};

	class Tokenizer {
	public:
		Tokenizer(const ReservedKeywordsPool& reserved, std::string input);
		std::vector<Token> tokenize() const;

	private:
		static std::unordered_set<std::string> BuildAllowedMultipleCharTokenSymbolsSet();

		RequiredToken determineCurrentToken(const std::size_t startIndex) const;
		std::pair<std::size_t, Token> tokenizeNext(const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		RequiredToken initializeCharType(const TokenType charTokenType) const;
		Token finalizeToken(std::size_t index, const RequiredToken& requiredToken, const std::size_t startIndex = 0) const;
		Token postComputing(std::size_t index, const RequiredToken& requiredToken, const std::size_t startIndex) const;
		TokenType calculateCharacterTokenType(const char c) const;
		static std::pair<ska::Token, ska::Token> stackToken(TokenType currentType, Token token, std::vector<Token>& stackSymbol);

		static bool isWordCharacter(const int c) {
			return std::isalnum(c) || c == '_';
		}
		
		static bool ignoreBlankToken(TokenType token) {
			return token != ska::TokenType::EMPTY &&
				token != ska::TokenType::SPACE;
		}

		static Token group(std::vector<Token>& symbolStack);
		static void push(Token t, std::vector<Token>& output);

		static SymbolFinalizeTokenState isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack);

		static int stopSymbolCharAggregation(char symbol);
		
		static const std::unordered_set<std::string> ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS;
		const ReservedKeywordsPool& m_reserved;
		std::string m_input;
	};
}
