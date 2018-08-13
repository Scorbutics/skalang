#include <iostream>

#include "Tokenizer.h"


const std::unordered_set<std::string> ska::Tokenizer::ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS = BuildAllowedMultipleCharTokenSymbolsSet();

std::unordered_set<std::string> ska::Tokenizer::BuildAllowedMultipleCharTokenSymbolsSet() {
	auto result = std::unordered_set<std::string> {};
	result.insert("+=");
	result.insert("-=");
	result.insert("*=");
	result.insert("/=");
	result.insert("!=");
	result.insert("==");
	result.insert(">=");
	result.insert("<=");
	result.insert("++");
	result.insert("--");
	result.insert("&=");
	result.insert("|=");
	return result;
}

ska::Tokenizer::Tokenizer(const ReservedKeywordsPool& reserved, std::string input) :
	m_reserved(reserved),
	m_input(std::move(input)) {
}

std::vector<ska::Token> ska::Tokenizer::tokenize() const {
	auto currentTokenToRead = determineCurrentToken(0);

	auto stackSymbol = std::vector<Token> {};
	auto result = std::vector<Token>{};
	auto startIndex = 0u;
	do {
		auto token = tokenizeNext(currentTokenToRead, startIndex);
		const auto tokenContent = token.type() == TokenType::RESERVED ? m_reserved.patternString(std::get<std::size_t>(token.content())) : token.asString();

		startIndex += tokenContent.size();

		if (currentTokenToRead.current != ska::TokenType::EMPTY && currentTokenToRead.current != ska::TokenType::SPACE) {	
			if(currentTokenToRead.current == ska::TokenType::SYMBOL) {
				if(isFinalizedSymbol(token.asString()[0], stackSymbol)) {
					push(std::move(token), stackSymbol, result);	
				} else {
					stackSymbol.push_back(std::move(token));
				}
			} else {
				push(std::move(token), stackSymbol, result);
			}
		}

		currentTokenToRead = determineCurrentToken(startIndex);
	} while (currentTokenToRead.current != ska::TokenType::EMPTY);
	return result;
}

void ska::Tokenizer::push(Token t, std::vector<Token>& symbolStack, std::vector<Token>& output) {
	if(!symbolStack.empty()) {
		auto ss = std::stringstream {}; 
		for(const auto& s : symbolStack) {
			ss << s.asString();
		}
		auto symbolToken = ska::Token { ss.str(), TokenType::SYMBOL };
		output.push_back(std::move(symbolToken));
		symbolStack.clear();
	}
	if(!t.empty()) {
		output.push_back(std::move(t));
	}
}

ska::RequiredToken ska::Tokenizer::determineCurrentToken(const std::size_t startIndex) const {
	if (startIndex < m_input.size()) {
		const auto charTokenType = calculateCharacterTokenType(m_input[startIndex]);
		const auto requiredToken = initializeCharType(charTokenType);
		return requiredToken;
	}
	return RequiredToken{};
}

//Handles "++", "--", "/=", "-=", "*=", ... has an unique token of symbol type instead of 2
bool ska::Tokenizer::isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack) {
	if(symbolStack.size() >= 2) {
		return true;
	}
	
	if(symbolStack.empty()) {
		return false;
	}

	const auto& topStackSymbol = symbolStack[0].asString()[0];
	if(stopSymbolCharAggregation(topStackSymbol) < 0) {
		return true;
	}

	if(stopSymbolCharAggregation(nextSymbol) < 1) {
		return true;
	}

	auto symbol = std::string{};
	symbol.resize(2);
	symbol[0] = topStackSymbol;
	symbol[1] = nextSymbol;
	std::cout << "Stack symbol : " << symbol;
	const auto result = ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.find(symbol) == ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.end();
	std::cout << " which is " << (!result ? "correct" : "incorrect") << std::endl;
	return result;
}

int ska::Tokenizer::stopSymbolCharAggregation(char symbol) {
	switch(symbol) {
		case '+':
		case '-':
		case '=':
			return 1;
		case '/':
		case '*':
		case '!':
		case '<':
		case '>':
		case '&':
		case '|':
			return 0;
		default:
			return -1;
	}
}

ska::Token ska::Tokenizer::tokenizeNext(const ska::RequiredToken& requiredToken, const std::size_t startIndex) const {
	auto index = startIndex == 0 ? 0 : startIndex + 1;
	if (index < m_input.size()) {
		auto charTokenType = requiredToken.current;
		for (; index < m_input.size(); index++) {
			charTokenType = calculateCharacterTokenType(m_input[index]);
			if (index > startIndex && requiredToken.required ^ requiredToken.requiredOrUntil[static_cast<std::size_t>(charTokenType)]) {
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
	case ska::TokenType::DOT_SYMBOL:
	case ska::TokenType::DIGIT:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DIGIT)] = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DOT_SYMBOL)] = true;
		break;

	case ska::TokenType::SPACE:
		required.required = true;
		break;

	case ska::TokenType::SYMBOL:
		required.required = true;
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
	case '.':
		return ska::TokenType::DOT_SYMBOL;
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
