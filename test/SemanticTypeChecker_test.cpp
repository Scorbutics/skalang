#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::Parser>;
using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticTC(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test, SemanticTypeCheckerPtr& semanticTypeChecker_test) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	parser_test = std::make_unique<ska::Parser> ( reservedKeywords, reader );
	semanticTypeChecker_test = std::make_unique<ska::SemanticTypeChecker>(*parser_test, *parser_test);
    table_test = std::make_unique<ska::SymbolTable> (*parser_test, *parser_test, *parser_test);
	semanticTypeChecker_test->setSymbolTable(*table_test);
    auto tokenTree = parser_test->parse();
	return tokenTree;
}

TEST_CASE("Semantic type checker") {
    ParserPtr parser_test;
    SemanticTypeCheckerPtr type_test;
    SymbolTablePtr table_test;
    
    SUBCASE("OK") {
        SUBCASE("float x float") {
            ASTFromInputSemanticTC("var titi = 2.0; var toto = 5.2; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("int x string") {    
            ASTFromInputSemanticTC("var titi = 2; var toto = \"test\"; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("int x float") {
            ASTFromInputSemanticTC("var titi = 2; var toto = 5.2; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("float x int") {
            ASTFromInputSemanticTC("var titi = 2.0; var toto = 5; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("string x int") {    
            ASTFromInputSemanticTC("var titi = \"tititititit\"; var toto = 6; titi * toto;", parser_test, table_test, type_test);
        }

    }

    SUBCASE("Fail") {
        SUBCASE("string x string") {
            try {    
                ASTFromInputSemanticTC("var titi = \"strrrr\"; var ttt = \"looool\"; ttt * titi;", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }  
        }

        SUBCASE("float x string") {
            try {
                ASTFromInputSemanticTC("var titi = 0.1; var toto = \"test\"; titi * toto;", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

        SUBCASE("Because of non-matching type (variable then function)") {
				try {
					ASTFromInputSemanticTC("var i = 120; i = function() {};", parser_test, table_test, type_test);
				    CHECK(false);
                } catch(std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Because of non-matching type (function then variable)") {
				try {
					ASTFromInputSemanticTC("var titi = function() {}; titi = 9;", parser_test, table_test, type_test);
				    CHECK(false);
                } catch(std::exception& e) {
					CHECK(true);
				}
			}
    }
}
