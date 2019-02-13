#pragma once
#include <vector>
#include "NodeValue/Token.h"

namespace ska {
    class TokenReader {
    public:
        TokenReader(const std::vector<ska::Token>& input, std::size_t startIndex = 0) :
            m_input{& input },
            m_lookAheadIndex{ startIndex } {
			if (!m_input->empty()) {
				m_lookAhead = &(*m_input)[m_lookAheadIndex];
			}
        }

		//std::pair<const std::vector<ska::Token>*, std::size_t> setSource(const std::vector<ska::Token>& input, std::size_t lookAheadIndex = 0);

		const Token& match(Token t);
		const Token& match(const TokenType type);
		bool expect(const Token& token) const;
		bool expect(const TokenType& type) const;
		bool empty() const;
		Token actual() const;
        const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;

    private:
        void nextToken();
        void error(Token* token = nullptr);

        const std::vector<ska::Token>* m_input;
        std::size_t m_lookAheadIndex {};
        const Token* m_lookAhead {};
    };
}
