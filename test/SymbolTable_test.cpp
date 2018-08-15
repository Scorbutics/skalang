#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
SymbolTablePtr table_test;

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	auto parser = ska::Parser { reservedKeywords, reader };
	table_test = std::make_unique<ska::SymbolTable> (parser, parser);
	auto tokenTree = parser.parse();
	return std::move(tokenTree.first);
}

TEST_CASE("test") {
	std::cout << std::endl << "symbol table" << std::endl;
	auto astPtr = ASTFromInput("{ var i = 0; { var toto = 2; var i = 9; }}");
	auto& table = *table_test;
	std::cout << "ok" << std::endl;

	CHECK(table.nested().size() == 1);
	CHECK(table["i"] != nullptr);
}
