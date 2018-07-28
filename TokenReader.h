#pragma once
#include <vector>
#include "Token.h"

namespace ska {
    class TokenReader {
    public:
        TokenReader(const std::vector<ska::Token>& input, std::size_t startIndex = 0) :
            m_input{ input },
            m_lookAheadIndex{ startIndex } {
			if (!m_input.empty()) {
				m_lookAhead = &m_input[m_lookAheadIndex];
			}
        }

		const Token& match(Token t);
		const Token& match(const TokenType type);
		bool expect(const Token& token) const;
		bool expect(const TokenType& type) const;
		bool empty() const;
		Token actual() const;

    private:
        void nextToken();
        static void error();

        std::size_t m_lookAheadIndex {};
        const Token* m_lookAhead {};
        const std::vector<ska::Token>& m_input;
    };
}
