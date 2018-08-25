#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::Parser>;
using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test, SemanticTypeCheckerPtr& semanticTypeChecker_test) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	parser_test = std::make_unique<ska::Parser> ( reservedKeywords, reader );
	semanticTypeChecker_test = std::make_unique<ska::SemanticTypeChecker>(*parser_test, *parser_test, *parser_test);
    table_test = std::make_unique<ska::SymbolTable> (*parser_test, *parser_test, *parser_test);
	semanticTypeChecker_test->setSymbolTable(*table_test);
    auto tokenTree = parser_test->parse();
	return tokenTree;
}

TEST_CASE("test") {
	ParserPtr parser_test;
	{
        SemanticTypeCheckerPtr type_test;
		SymbolTablePtr table_test;
		auto astPtr = ASTFromInput("var i = 0; var titi = \"llllll\"; { var toto = 2; var i = 9; }", parser_test, table_test, type_test);
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

		SUBCASE("Overriding into subscope") {
            SemanticTypeCheckerPtr type_test;
			SymbolTablePtr table_test;
			auto astPtr = ASTFromInput("var i = 0; i = 123; { i = 9; }", parser_test, table_test, type_test);
			auto& table = *table_test;
			
			CHECK(table.nested().size() == 1);
			auto nestedI = (*table.nested()[0])["i"];
			auto i = table["i"];

			CHECK(i != nullptr);
		}

        SUBCASE("Unknown symbol outside when declared as function parameter") {
            SemanticTypeCheckerPtr type_test;
            SymbolTablePtr table_test;
            std::cout << std::endl << std::endl << std::endl;
            ASTFromInput("var func = function(test:int) { test = 123; };", parser_test, table_test, type_test);
        }           
	}

	SUBCASE("Matching failed") {
		ParserPtr parser_test;
		{
			SUBCASE("Because of unknown symbol") {
				SemanticTypeCheckerPtr type_test;
                SymbolTablePtr table_test;
				try {
					ASTFromInput("var i = 0; var titi = \"llllll\"; { ti = 9; }", parser_test, table_test, type_test);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Unknown symbol outside when declared as function parameter") {
				SemanticTypeCheckerPtr type_test;
                SymbolTablePtr table_test;
				try {
					ASTFromInput("var func = function(test:int) {}; test = 123;", parser_test, table_test, type_test);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}
        }   
	}
}
