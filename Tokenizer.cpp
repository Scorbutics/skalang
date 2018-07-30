#include "Tokenizer.h"


ska::Tokenizer::Tokenizer(const ReservedKeywordsPool& reserved, std::string input) :
	m_reserved(reserved),
	m_input(std::move(input)) {
}

std::vector<ska::Token> ska::Tokenizer::tokenize() const {
	auto currentTokenToRead = determineCurrentToken(0);

	auto result = std::vector<Token>{};
	auto startIndex = 0u;
	do {
		auto token = tokenizeNext(currentTokenToRead, startIndex);

		startIndex += token.type() == TokenType::RESERVED ? m_reserved.pattern(std::get<std::size_t>(token.content())).asString().size() : std::get<std::string>(token.content()).size();
		if (currentTokenToRead.current != ska::TokenType::EMPTY && currentTokenToRead.current != ska::TokenType::SPACE) {
			result.push_back(std::move(token));
		}
		currentTokenToRead = determineCurrentToken(startIndex);
	} while (currentTokenToRead.current != ska::TokenType::EMPTY);
	return result;
}

ska::RequiredToken ska::Tokenizer::determineCurrentToken(const std::size_t startIndex) const {
	if (startIndex < m_input.size()) {
		const auto charTokenType = calculateCharacterTokenType(m_input[startIndex]);
		auto requiredToken = initializeCharType(charTokenType);
		return requiredToken;
	}
	return RequiredToken{};
}

ska::Token ska::Tokenizer::tokenizeNext(const ska::RequiredToken& requiredToken, const std::size_t startIndex) const {
	auto index = startIndex == 0 ? 0 : startIndex + 1;
	if (index < m_input.size()) {
		auto charTokenType = requiredToken.current;
		for (; index < m_input.size(); index++) {
			charTokenType = calculateCharacterTokenType(m_input[index]);
			if (requiredToken.required ^ requiredToken.requiredOrUntil[static_cast<std::size_t>(charTokenType)]) {
				if (index == 0) {
					index++;
				}
				break;
			}
		}
	} else {
		index = m_input.size();
	}
	return finalizeToken(index, requiredToken, startIndex);
}

ska::RequiredToken ska::Tokenizer::initializeCharType(const ska::TokenType charTokenType) const {
	auto required = ska::RequiredToken{};
	required.current = charTokenType;
	switch (charTokenType) {
	case ska::TokenType::DIGIT:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DIGIT)] = true;
		break;

	case ska::TokenType::SPACE:
		required.required = true;
		break;

	case ska::TokenType::SYMBOL:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::SYMBOL)] = true;
		break;

	case ska::TokenType::RANGE:
		required.required = true;
		break;

	case ska::TokenType::STRING:
		required.required = false;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::STRING)] = true;
		break;

	default:
	case ska::TokenType::IDENTIFIER:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DIGIT)] = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::IDENTIFIER)] = true;
		break;
	}
	return required;
}

ska::Token ska::Tokenizer::finalizeToken(std::size_t index, const ska::RequiredToken& requiredToken, const std::size_t startIndex) const {
	auto token = ska::Token{};
	index += (!requiredToken.required ? 1 : 0);
	if (index != startIndex) {
		token.init(m_input.substr(startIndex, index - startIndex), requiredToken.current);
		if (token.type() == ska::TokenType::IDENTIFIER && m_reserved.pool.find(std::get<std::string>(token.content())) != m_reserved.pool.end()) {
			token = m_reserved.pool.at(std::get<std::string>(token.content())).token;
		}
	}
	return token;
}

ska::TokenType ska::Tokenizer::calculateCharacterTokenType(const char c) const {
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return ska::TokenType::DIGIT;

	case ' ':
	case '\t':
	case '\v':
	case '\f':
	case '\n':
	case '\r':
		return ska::TokenType::SPACE;

	case '"':
		return ska::TokenType::STRING;

	case '(':
	case ')':
	case '{':
	case '}':
		return ska::TokenType::RANGE;

	default:
		return isWordCharacter(c) ? ska::TokenType::IDENTIFIER : ska::TokenType::SYMBOL;
	}
}