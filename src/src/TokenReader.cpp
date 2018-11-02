#include "TokenReader.h"

const ska::Token& ska::TokenReader::match(Token t) {
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
    auto t = Token { ss.str(), TokenType::IDENTIFIER };
    error(&t);
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

bool ska::TokenReader::expect(const TokenType& tokenType) const {
    return m_lookAhead != nullptr && m_lookAhead->type() == tokenType;
}

bool ska::TokenReader::empty() const {
    return m_lookAhead == nullptr || m_lookAhead->type() == ska::TokenType::EMPTY;
}

const ska::Token& ska::TokenReader::readPrevious(std::size_t offset) const {
    if(m_lookAheadIndex < offset) {
        auto ss = std::stringstream {};
        ss << "unable to rollback the current token stream from an offset of " << offset << " because the lookahead is only at an index of " << m_lookAheadIndex;
        throw std::runtime_error(ss.str());
    }

    return m_input[m_lookAheadIndex - offset];
}

void ska::TokenReader::error(Token* token) {
    throw std::runtime_error("syntax error : bad token matching : expected " + (token == nullptr ? "UNKNOWN_TOKEN" : token->asString()) + " but got \"" + (m_lookAhead == nullptr ? "EMPTY_TOKEN (no more lookahead)" : m_lookAhead->asString()) + "\"");
}

void ska::TokenReader::nextToken() {
    m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
}