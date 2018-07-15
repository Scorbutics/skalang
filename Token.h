#pragma once
#include <string>
#include <variant>
#include <sstream>
#include <cassert>

namespace ska {
	constexpr const char* TokenTypeSTR[] = {
	"RESERVED",
	"IDENTIFIER",
	"DIGIT",
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
		SPACE,
		STRING,
		RANGE,
		SYMBOL,
		EMPTY,
		UNUSED_LAST_Length
	};

	struct Token {
		using Variant = std::variant<std::size_t, double, std::string>;
		
		Token() {
			//std::cout << "Empty " << asString() << std::endl;
		}
		
		Token(const Token& t) {
			*this = t;
		}
		
		Token(Token&& t) noexcept {
			*this = std::move(t);
		}
		
		Token& operator=(const Token& t) {
			m_content = t.m_content;
			m_type = t.m_type;
			//std::cout << "Copy " << asString() << std::endl;
			return *this;
		}
		
		Token& operator=(Token&& t) noexcept {
			m_content = std::move(t.m_content);
			m_type = std::move(t.m_type);
			//std::cout << "Move " << asString() << std::endl;
			return *this;
		}
		
		Token(std::string c, TokenType t) : 
			m_type(std::move(t)) {
			init(c, m_type);
			//std::cout << "From string" << asString() << std::endl;
		}

		Token(std::size_t c, TokenType t) :
			m_type(std::move(t)) {
			if (m_type == TokenType::EMPTY) {
				m_content = "";
			} else {
				init(c, m_type);
			}
			//std::cout << "From int " << asString() << std::endl;
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

		std::string asString() const {
			if(m_type == TokenType::EMPTY) {
				return "";
			}
			
			if(m_type == TokenType::RESERVED) {
				std::stringstream ss;
				ss << std::get<std::size_t>(m_content);
				return ss.str();
			}
			
			//std::cout << TokenTypeSTR[static_cast<int>(m_type)] << std::endl;
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