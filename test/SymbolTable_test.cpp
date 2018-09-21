#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "DataTestContainer.h"

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	data.parser = std::make_unique<ska::Parser> ( reservedKeywords, reader );
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser);
    data.symbols = std::make_unique<ska::SymbolTable> (*data.parser);
	data.typeChecker->setSymbolTable(*data.symbols);
    
    return data.parser->parse();
}

TEST_CASE("test") {
    DataTestContainer data;
    auto astPtr = ASTFromInput("var i = 0; var titi = \"llllll\"; { var toto = 2; var i = 9; }", data);
    auto& table = *data.symbols;
    
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

TEST_CASE("Matching") {
	
	SUBCASE("Matching OK") {
        DataTestContainer data;
		
        SUBCASE("Overriding into subscope") {
			auto astPtr = ASTFromInput("var i = 0; i = 123; { i = 9; }", data);
			auto& table = *data.symbols;
			
			CHECK(table.nested().size() == 1);
			auto nestedI = (*table.nested()[0])["i"];
			auto i = table["i"];

			CHECK(i != nullptr);
		}

        SUBCASE("var in upper scope used into inner function scope") {
            ASTFromInput("var test = 21; var func = function() { test = 123; };", data);
        }

        SUBCASE("function parameter use into function") {
            ASTFromInput("var func = function(test:int) { test = 123; };", data);
        }
        
        SUBCASE("function declared and used in another function with upper variable") {
            ASTFromInput("var func = function(test:int) { var toutou = function(blurp:string) { test = 123; }; test = 78; toutou(\"llll\"); };", data);
        }

        SUBCASE("shadowing variable into inner function") {
            ASTFromInput("var test = 3; var func = function(test:string) { test; };", data);
        }
        
	}

	SUBCASE("Matching failed") {
		DataTestContainer data;
        {
			SUBCASE("Because of unknown symbol") {
				try {
					ASTFromInput("var i = 0; var titi = \"llllll\"; { ti = 9; }", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Unknown symbol outside when declared as function parameter") {
				try {
					ASTFromInput("var func = function(test:int) {}; test = 123;", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

            SUBCASE("used out of function scope") {
                try {
                    ASTFromInput("var func = function(test:int) { var tout = function(blurp:string) {};}; tout = 332;", data);
                    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
            }
        }   
	}
}
