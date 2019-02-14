#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "DataTestContainer.h"
#include "Service/Script.h"

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script { tokens };
	data.parser = std::make_unique<ska::StatementParser> ( reservedKeywords );
	data.symbols = std::make_unique<ska::SymbolTable>(*data.parser);

    return data.parser->parse(reader);
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
            ASTFromInput("var test59 = 21; var func59 = function() { test59 = 123; };", data);
        }

        SUBCASE("function parameter use into function") {
            ASTFromInput("var func63 = function(test63:int) { test63 = 123; };", data);
        }
        
        SUBCASE("function declared in another function with upper variable") {
            ASTFromInput("var func67 = function(testParam67:int) { var toutou67 = function(blurp:string) { testParam67 = 123; }; testParam67 = 78; };", data);
        }

        SUBCASE("shadowing variable into inner function") {
            ASTFromInput("var test71 = 3; var func71 = function(test71:string) { test71; };", data);
        }
        
	}

	SUBCASE("Matching failed") {
		DataTestContainer data;
        {
			SUBCASE("Because of unknown symbol") {
				try {
					ASTFromInput("var i81 = 0; var titi81 = \"llllll\"; { ti81 = 9; }", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Unknown symbol outside when declared as function parameter") {
				try {
					ASTFromInput("var func90 = function(test90:int) {}; test90 = 123;", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

            SUBCASE("used out of function scope") {
                try {
                    ASTFromInput("var func99 = function(test99:int) { var tout99 = function(blurp99:string) {};}; tout99 = 332;", data);
                    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
            }
        }   
	}
}
