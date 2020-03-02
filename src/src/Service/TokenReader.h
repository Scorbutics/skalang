#pragma once
#include <vector>
#include <functional>
#include "NodeValue/Token.h"

namespace ska {
	using TokenReaderExpectCallback = std::function<bool(const Token&)>;

	struct TokenReaderExpect {
		static TokenReaderExpectCallback value(const Token::Variant& expectedValue){
			return [&expectedValue] (const Token & value) {
				return expectedValue == value.content();
			};
		}

		static TokenReaderExpectCallback type(const TokenType& expectedValue) {
			return [&expectedValue](const Token& value) {
				return expectedValue == value.type();
			};
		}
	};

	class TokenReader {
	public:
    	TokenReader(std::vector<ska::Token> input, std::size_t startIndex = 0) :
        	m_input(std::move(input)),
        	m_lookAheadIndex{ startIndex } {
			if (!m_input.empty()) {
				m_lookAhead = &m_input[m_lookAheadIndex];
			}
    	}

		const Token* mightMatch(const Token& token);
		const Token& match(const Token& t);
		const Token& match(const TokenType type);
		bool expect(const Token& token) const;
		bool expect(const TokenType& type) const;
		bool empty() const;
		Token actual() const;
    	const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;
		void rewind();
		bool ahead(const TokenReaderExpectCallback& callback, std::size_t offset = 1u) const;
		bool ahead(const Token& expected, std::size_t offset = 1u) const;

	private:
    	void nextToken();
		[[nodiscard]]
		const Token* nextToken(std::size_t offset) const;
    	void error(const Token* token = nullptr);

    	const std::vector<ska::Token> m_input;
    	std::size_t m_lookAheadIndex {};
    	const Token* m_lookAhead {};
	};
}
