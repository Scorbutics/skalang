#include "TokenReader.h"

const ska::Token& ska::TokenReader::match(const Token& t) {
	if (m_lookAhead != nullptr && *m_lookAhead == t) {
    	return match(t.type());
	}
	error(&t);
	throw std::runtime_error("unexpected error");
}

const ska::Token& ska::TokenReader::match(const TokenType type) {
	if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
    	const auto& result = *m_lookAhead;
    	nextToken();
    	return result;
	}
	auto ss = std::stringstream {};
	ss << "a token with type \"" << TokenTypeSTR[static_cast<std::size_t>(type)] << "\"" ;
	auto t = Token { ss.str(), TokenType::IDENTIFIER, (m_lookAhead != nullptr ? m_lookAhead->position() : Cursor {}) };
	error(&t);
	throw std::runtime_error("unexpected error");
}

bool ska::TokenReader::ahead(const TokenReaderExpectCallback& callback, std::size_t offset) const {
	const auto* token = nextToken(offset);	
	return token != nullptr && callback(*token);
}

bool ska::TokenReader::ahead(const Token& expected, std::size_t offset) const {
	const auto* token = nextToken(offset);
	return token != nullptr && *token == expected;
}

void ska::TokenReader::rewind() {
	if (!m_input.empty()) {
		m_lookAheadIndex = 0;
		m_lookAhead = &m_input[m_lookAheadIndex];
	}
}

ska::Token ska::TokenReader::actual() const {
	if (m_lookAhead != nullptr) {
    	return *m_lookAhead;
	}
	return Token {};
}

bool ska::TokenReader::expect(const Token& token) const {
	return m_lookAhead != nullptr && (*m_lookAhead) == token;
}

const ska::Token* ska::TokenReader::mightMatch(const Token& token) {
	return expect(token) ? &match(token) : nullptr;
}

bool ska::TokenReader::expect(const TokenType& tokenType) const {
	return m_lookAhead != nullptr && m_lookAhead->type() == tokenType;
}

bool ska::TokenReader::empty() const {
	return m_lookAhead == nullptr || m_lookAhead->type() == ska::TokenType::EMPTY;
}

bool ska::TokenReader::emptyTokens() const {
	return m_input.empty();
}

bool ska::TokenReader::canReadPrevious(std::size_t offset) const {
	return m_lookAheadIndex >= offset;
}

const ska::Token& ska::TokenReader::readPrevious(std::size_t offset) const {
	if(m_lookAheadIndex < offset) {
    	auto ss = std::stringstream {};
    	ss << "unable to rollback the current token stream from an offset of " << offset << " because the lookahead is only at an index of " << m_lookAheadIndex;
    	throw std::runtime_error(ss.str());
	}

	return m_input[m_lookAheadIndex - offset];
}

void ska::TokenReader::error(const Token* token) {
	throw std::runtime_error("syntax error : bad token matching : expected \"" + (token == nullptr ? "UNKNOWN_TOKEN" : token->name()) + "\" but got \"" + (m_lookAhead == nullptr ? "EMPTY_TOKEN (no more lookahead)" : m_lookAhead->name()) + "\"");
}

void ska::TokenReader::nextToken() {
	m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
}

const ska::Token* ska::TokenReader::nextToken(std::size_t offset) const {
	return (m_lookAheadIndex + offset) < m_input.size() ? &m_input[m_lookAheadIndex + offset] : nullptr;
}
