#include <iostream>
#include <sstream>
#include <iomanip>

#include "Tokenizer.h"
#include "Parser.h"

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

void PrintTokenTreeRPN(const std::unique_ptr<ska::ASTNode>& node) {	
	
	
	if(node->left != nullptr) {
		PrintTokenTreeRPN(node->left);
	}
	
	if(node->right != nullptr) {
		PrintTokenTreeRPN(node->right);
	}
	
	std::cout << node->token.asString() << " ";
	
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
			std::cout << "[" << std::setw(10) << TokenTypeSTR[static_cast<std::size_t>(token.type())] << "]\t" << (token.type() == ska::TokenType::RESERVED ? reservedKeywords.poolList[std::get<std::size_t>(token.content())] : std::get<std::string>(token.content())) << std::endl;
		}

		auto parser = ska::Parser { std::move(tokens) };
		auto tokenTree = parser.parse();
		
		//PrintTokenTreeRPN(tokenTree);
		std::cout << std::endl;
	}
    return 0;
}

