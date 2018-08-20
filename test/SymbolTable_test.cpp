#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::Parser>;

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	parser_test = std::make_unique<ska::Parser> ( reservedKeywords, reader );
	table_test = std::make_unique<ska::SymbolTable> (*parser_test, *parser_test, *parser_test);
	auto tokenTree = parser_test->parse();
	return std::move(tokenTree.first);
}

TEST_CASE("test") {
	ParserPtr parser_test;
	{
		SymbolTablePtr table_test;
		auto astPtr = ASTFromInput("var i = 0; var titi = \"llllll\"; { var toto = 2; var i = 9; }", parser_test, table_test);
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
}

TEST_CASE("Matching") {
	SUBCASE("Matching OK") {
		ParserPtr parser_test;
		{
			SymbolTablePtr table_test;
			auto astPtr = ASTFromInput("var i = 0; i = \"llllll\"; { i = 9; }", parser_test, table_test);
			auto& table = *table_test;
			
			CHECK(table.nested().size() == 1);
			auto nestedI = (*table.nested()[0])["i"];
			auto i = table["i"];

			CHECK(i != nullptr);
		}
	}

	SUBCASE("Matching failed") {
		ParserPtr parser_test;
		{
			SUBCASE("Because of unknown symbol") {
				SymbolTablePtr table_test;
				try {
					ASTFromInput("var i = 0; var titi = \"llllll\"; { ti = 9; }", parser_test, table_test);
				} catch (std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Because of non-matching type (variable then function)") {
				SymbolTablePtr table_test;
				ASTFromInput("var i = 0; i = function() {};", parser_test, table_test);
			}

			SUBCASE("Because of non-matching type (function then variable)") {
				SymbolTablePtr table_test;
				ASTFromInput("var titi = function() {}; titi = 9;", parser_test, table_test);
			}
		}
	}
}
