#include <iostream>
#include <sstream>
#include <iomanip>

#include "Tokenizer.h"
#include "Parser.h"
#include "TokenReader.h"
#include "SymbolTable.h"

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

void PrintTokenTree(const std::unique_ptr<ska::ASTNode>& node) {

    std::cout << node->token.asString() << " ";

	for (const auto& child : (*node)) {
		PrintTokenTree(child);
	}

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
		std::cout << "Tokenize : " << std::endl;
		const auto tokens = tokenizer.tokenize();
		for (const auto& token : tokens) {
			std::cout << "[" << std::setw(10) << TokenTypeSTR[static_cast<std::size_t>(token.type())] << "]\t" << (token.type() == ska::TokenType::RESERVED ? reservedKeywords.pattern(std::get<std::size_t>(token.content())).asString() : std::get<std::string>(token.content())) << std::endl;
		}

		auto reader = ska::TokenReader { tokens };
		auto parser = ska::Parser { reservedKeywords, reader };
		auto symbolTable = ska::SymbolTable { parser };
		auto tokenTree = parser.parse();

		PrintTokenTree(tokenTree.first);
		std::cout << std::endl;
	}
    return 0;
}

