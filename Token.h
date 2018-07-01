#pragma once
#include <string>
#include <variant>

namespace ska {
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
		Token() = default;
		Token(std::string c, TokenType type) : 
			type(std::move(type)) {
			content = c;
		}

		Token(std::size_t c, TokenType type) :
			type(std::move(type)) {
			content = c;
			if (type == TokenType::EMPTY) {
				content = "";
			}
		}

		std::variant<std::size_t, double, std::string> content;
		TokenType type = TokenType::EMPTY;

		bool operator==(const Token& t1) const {
			return type == t1.type && content == t1.content;
		}
	};

	/*struct TokenNode {
		TokenNode() = default;
		TokenNode(Token token) : token(std::move(token)) {}

		Token token;
		std::vector<TokenNode> subTokens;
	};*/
};