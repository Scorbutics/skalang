#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "Tokenizer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Tokenizer)

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

std::pair<ska::Token, ska::Token> ska::Tokenizer::stackToken(TokenType currentType, Token token, std::vector<Token>& stackSymbol) {
	if (currentType == ska::TokenType::SYMBOL) {
		const auto charToken = token.name()[0];
		switch (isFinalizedSymbol(charToken, stackSymbol)) {
		case SymbolFinalizeTokenState::READY:
			stackSymbol.push_back(std::move(token));
			return {};
		case SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL: {
			auto result = std::make_pair(group(stackSymbol), Token{});
			stackSymbol.push_back(std::move(token));
			return result;
		}
		case SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL:
			stackSymbol.push_back(std::move(token));
			return std::make_pair(group(stackSymbol), Token{});			
		default:
			assert(!"Unhandled token finalization state");
			return {};
		} 
	}

	return std::make_pair(group(stackSymbol), std::move(token));
}

ska::Cursor ska::Tokenizer::computeTokenPositionCursor(std::size_t index, const Token& readToken, bool wasRequired, const Cursor& lastCursor) {
	const auto rawIndex = (wasRequired ? 0 : 1 ) + index;
	const auto isEOL = readToken.type() == TokenType::SPACE && std::get<std::string>(readToken.content()) == "\n";
	const auto tokenLength = readToken.name().size();
	
	const auto column =  
		isEOL ? 1u : 
		(lastCursor.line == 1 ? 
			(static_cast<unsigned int>(rawIndex) + 1) : 
			lastCursor.column + static_cast<unsigned int>(tokenLength));

	const auto line = isEOL ? (lastCursor.line + 1) : lastCursor.line;
	
	SLOG_STATIC(LogLevel::Debug, Tokenizer) << "Token calculated \"" << readToken.name() << "\" at (l." << line << ", c. " << column << ")";

	return { 
		.rawIndex = rawIndex, 
		.column = column,
		.line = line
	};
}

std::vector<ska::Token> ska::Tokenizer::tokenize() const {
	auto currentTokenToRead = RequiredToken {};
	auto stackSymbol = std::vector<Token> {};
	auto result = std::vector<Token>{};

	auto cursor = Cursor {};
	auto token = Token {};
	do {
		currentTokenToRead = determineCurrentToken(cursor.rawIndex);
		std::tie(cursor, token) = tokenizeNext(currentTokenToRead, cursor);
		if (isNotBlankToken(currentTokenToRead.current)) {	
			auto group = stackToken(currentTokenToRead.current, std::move(token), stackSymbol);
			push(std::move(group.first), result);
			push(std::move(group.second), result);			
		}
	
	} while (currentTokenToRead.current != ska::TokenType::EMPTY);
	
	push(group(stackSymbol), result);
	
	return result;
}

ska::Token ska::Tokenizer::group(std::vector<Token>& symbolStack) {
	if (symbolStack.empty()) {
		return {};
	}

	auto ss = std::stringstream {}; 
	for(const auto& s : symbolStack) {
		ss << s;
	}
	auto tokenValueStr = ss.str();
	SLOG_STATIC(LogLevel::Info, Tokenizer) << "Pushing compound symbol \"" <<  tokenValueStr << "\"";
	auto symbolToken = ska::Token {std::move(tokenValueStr) , TokenType::SYMBOL, symbolStack.empty() ? Cursor {} : symbolStack[0].position() };
	symbolStack.clear();
	return symbolToken;
	
}

void ska::Tokenizer::push(Token t, std::vector<Token>& output) {
	if (!t.empty()) {
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
ska::SymbolFinalizeTokenState ska::Tokenizer::isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack) {
	if(symbolStack.size() >= 2) {
		return SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL;
	}
	
	if(symbolStack.empty()) {
		return SymbolFinalizeTokenState::READY;
	}

	const auto topStackSymbol = symbolStack[0].name()[0];
	if(stopSymbolCharAggregation(topStackSymbol) < 0) {
		return SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
	}

	if(stopSymbolCharAggregation(nextSymbol) < 1) {
		return SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
	}

	auto symbol = std::string{};
	symbol.resize(2);
	symbol[0] = topStackSymbol;
	symbol[1] = nextSymbol;
	SLOG_STATIC(LogLevel::Debug, Tokenizer) << "Stack symbol : " << symbol;
	const auto result = ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.find(symbol) != ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.end();
	SLOG_STATIC(LogLevel::Debug, Tokenizer) << " which is " << (result ? "correct" : "incorrect");
	return result ? SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL : SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
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

std::pair<ska::Cursor, ska::Token> ska::Tokenizer::tokenizeNext(const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	auto index = lastCursor.rawIndex == 0 ? 0u : lastCursor.rawIndex + 1;
	if (index < m_input.size()) {
		auto charTokenType = requiredToken.current;
		for (; index < m_input.size(); index++) {
			charTokenType = calculateCharacterTokenType(m_input[index]);
			if (index > lastCursor.rawIndex && requiredToken.required ^ requiredToken.requiredOrUntil[static_cast<std::size_t>(charTokenType)]) {
				if (index == 0) {
					index++;
				}
				break;
			}
		}
	} else {
		index = m_input.size();
	}
	auto token = finalizeToken(index, requiredToken, lastCursor);
	auto nextCursor = computeTokenPositionCursor(index, token, requiredToken.required, lastCursor);
	return std::make_pair(std::move(nextCursor), std::move(token));
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

	case ska::TokenType::ARRAY:
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

ska::Token ska::Tokenizer::postComputing(std::size_t index, const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	switch (requiredToken.current) {
	case TokenType::IDENTIFIER: {
		const auto value = m_input.substr(lastCursor.rawIndex, index - lastCursor.rawIndex);
		return m_reserved.pool.find(value) != m_reserved.pool.end() ? Token{ m_reserved.pool.at(value).token, lastCursor} : Token{value, requiredToken.current, lastCursor};
	}
	
	case TokenType::STRING:
		return Token{ m_input.substr(lastCursor.rawIndex + 1, index - lastCursor.rawIndex - 2), requiredToken.current, lastCursor };
	
	default:
		return Token{ m_input.substr(lastCursor.rawIndex, index - lastCursor.rawIndex), requiredToken.current, lastCursor };
	}
}

ska::Token ska::Tokenizer::finalizeToken(std::size_t index, const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	index += (!requiredToken.required ? 1 : 0);
	if (index != lastCursor.rawIndex) {
		return postComputing(index, requiredToken, lastCursor);
	}
	return {};
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

	case '[':
	case ']':
		return ska::TokenType::ARRAY;

	default:
		return isWordCharacter(c) ? ska::TokenType::IDENTIFIER : ska::TokenType::SYMBOL;
	}
}
