#include <iostream>
#include <sstream>
#include <iomanip>

#include "Tokenizer.h"

namespace ska {
	
	struct TokenNode {
		Token token;
		std::vector<TokenNode> children;
	};

	class Parser {
	public:
		Parser(std::vector<Token> input) :
			m_input(std::move(input)) {
		}

		TokenNode parse() const {
			auto root = TokenNode {};
			if (!m_input.empty()) {
				const auto& statement = m_input[0];
				switch (statement.type) {
				case TokenType::RESERVED:
					break;
				}
			}
			return root;
		}

	private:
		std::vector<Token> m_input;
	};
}

int main() {
	for (;;) {
		auto stop = false;
		std::stringstream ss;

		while (!stop) {
			auto line = std::string{};
			std::getline(std::cin, line);
			stop = line.empty();
			ss << line;
		}

		const auto reservedKeywords = ska::ReservedKeywordsPool{};

		auto tokenizer = ska::Tokenizer { reservedKeywords, ss.str() };
		auto tokens = tokenizer.tokenize();
		for (const auto& token : tokens) {
			std::cout << "[" << std::setw(10) << ska::TokenTypeSTR[static_cast<std::size_t>(token.type)] << "]\t" << token.content << std::endl;
		}

		auto parser = ska::Parser { std::move(tokens) };
		auto tokenTree = parser.parse();

		std::cout << std::endl;
	}
    return 0;
}

