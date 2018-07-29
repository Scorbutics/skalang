#include "TokenReader.h"

const ska::Token& ska::TokenReader::match(Token t) {
    if (m_lookAhead != nullptr && *m_lookAhead == t) {
        return match(t.type());
    }
    error();
	throw std::runtime_error("unexpected error");
}

const ska::Token& ska::TokenReader::match(const TokenType type) {
    if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
        const auto& result = *m_lookAhead;
        nextToken();
        return result;
    }
    error();
	throw std::runtime_error("unexpected error");
}

ska::Token ska::TokenReader::actual() const {
    if (m_lookAhead != nullptr) {
        return *m_lookAhead;
    }
    return Token {"", TokenType::EMPTY};
}

bool ska::TokenReader::expect(const Token& token) const {
    return m_lookAhead != nullptr && (*m_lookAhead) == token;
}

void ska::TokenReader::rollbackOne() {
	if (m_lookAheadIndex > 0) {
		m_lookAhead = &m_input[--m_lookAheadIndex];
	}
}

bool ska::TokenReader::expect(const TokenType& tokenType) const {
    return m_lookAhead != nullptr && m_lookAhead->type() == tokenType;
}

bool ska::TokenReader::empty() const {
    return m_lookAhead == nullptr || m_lookAhead->type() == ska::TokenType::EMPTY;
}

void ska::TokenReader::error() {
    throw std::runtime_error("syntax error : bad token matching");
}

void ska::TokenReader::nextToken() {
    m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
}
