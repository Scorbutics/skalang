#pragma once
#include <string>
#include <variant>
#include <sstream>
#include <cassert>

#include "TokenGrammar.h"

namespace ska {
	constexpr const char* TokenTypeSTR[] = {
	"RESERVED",
	"IDENTIFIER",
	"DIGIT",
	"DOT_SYMBOL",
	"SPACE",
	"STRING",
	"RANGE",
	"SYMBOL",
	"EMPTY",
	"UNUSED_LAST_Length"
	};

	enum class TokenType {
		RESERVED,
		IDENTIFIER,
		DIGIT,
		DOT_SYMBOL,
		SPACE,
		STRING,
		RANGE,
		SYMBOL,
		EMPTY,
		UNUSED_LAST_Length
	};

	struct Token {
		using Variant = std::variant<std::size_t, double, std::string>;

		Token() = default;
		Token(const Token& t) = default;
		Token(Token&& t) noexcept = default;
		Token& operator=(const Token& t) = default;
		Token& operator=(Token&& t) noexcept = default;

		Token(std::string c, TokenType t) :
			m_type(std::move(t)) {
			init(c, m_type);
		}

		Token(std::size_t c, TokenType t) :
			m_type(std::move(t)) {
			if (m_type == TokenType::EMPTY) {
				m_content = "";
			} else {
				init(c, m_type);
			}
		}

		void init(std::size_t c, TokenType t) {
			m_type = t;
			assert(m_type == TokenType::RESERVED);
			m_content = c;
		}

		void init(std::string c, TokenType t) {
			m_type = t;
			assert(m_type != TokenType::RESERVED && m_type != TokenType::UNUSED_LAST_Length);
			m_content = std::move(c);
		}

		const Variant& content() const {
			return m_content;
		}

		const TokenType& type() const {
			return m_type;
		}

        const bool isLiteral() const {
            switch(m_type) {
                case TokenType::DIGIT:
                case TokenType::STRING:
                    return true;
                default:
                    return false;
            }
        }

		std::string asString() const {
			if(m_type == TokenType::EMPTY) {
				return "EMPTY_TOKEN";
			}

			if(m_type == TokenType::RESERVED) {
				std::stringstream ss;
				ss << TokenGrammarSTR[std::get<std::size_t>(m_content)];
				return ss.str();
			}

			return std::holds_alternative<std::string>(m_content) ? std::get<std::string>(m_content) : "error variant";
		}

		bool empty() const {
			return m_type == TokenType::EMPTY;
		}

		bool operator==(const Token& t1) const {
			return m_type == t1.m_type && m_content == t1.m_content;
		}

		bool operator!=(const Token& t1) const {
			return m_type != t1.m_type || m_content != t1.m_content;
		}

	private:
		Variant m_content = std::string{};
		TokenType m_type = TokenType::EMPTY;
	};

};
