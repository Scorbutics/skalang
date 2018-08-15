#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, SymbolTablePtr& table_test) {
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
	SymbolTablePtr table_test;
	auto astPtr = ASTFromInput("var i = 0; var titi = \"llllll\"; { var toto = 2; var i = 9; }", table_test);
	auto& table = *table_test;
	
	CHECK(table.nested().size() == 1);
	auto nestedI = (*table.nested()[0])["i"];
	auto i = table["i"];
	auto nestedToto = (*table.nested()[0])["toto"];
	auto toto = table["toto"];
	auto titi = table["titi"];
	auto nestedTiti = (*table.nested()[0])["titi"];

	CHECK(i != nullptr);
	CHECK(nestedI  != nullptr);
	CHECK(i != nestedI);
	CHECK(toto == nullptr);
	CHECK(nestedToto != nullptr);
	CHECK(nestedTiti != nullptr);
	CHECK(nestedTiti == titi);
}
