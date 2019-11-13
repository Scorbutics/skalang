#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "DataTestContainer.h"
#include "NodeValue/ScriptAST.h"

auto reader = std::unique_ptr<ska::ScriptAST>{};
ska::ScriptAST ASTFromInput(ska::ScriptCacheAST& scriptCache, const std::string& input, DataTestContainer& data) {
	auto tokenizer = ska::Tokenizer { data.reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	reader = std::make_unique<ska::ScriptAST>(scriptCache, "main", tokens);
	data.parser = std::make_unique<ska::StatementParser> ( data.reservedKeywords );
    reader->parse(*data.parser);
    return *reader;
}

TEST_CASE("test") {
    DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

    auto astPtr = ASTFromInput(scriptCache, "var i = 0; var titi = \"llllll\"; { var toto = 2; var i = 9; }", data);
    auto& table = reader->symbols();
    
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
	reader = nullptr;
}

TEST_CASE("Matching") {
	
	SUBCASE("Matching OK") {
        DataTestContainer data;
		auto scriptCache = ska::ScriptCacheAST{};
        SUBCASE("Overriding into subscope") {
			auto astPtr = ASTFromInput(scriptCache, "var i = 0; i = 123; { i = 9; }", data);
			auto& table = reader->symbols();
			
			CHECK(table.nested().size() == 1);
			auto nestedI = (*table.nested()[0])["i"];
			auto i = table["i"];

			CHECK(i != nullptr);
		}

        SUBCASE("var in upper scope used into inner function scope") {
            ASTFromInput(scriptCache, "var test59 = 21; var func59 = function() { test59 = 123; };", data);
        }

        SUBCASE("function parameter use into function") {
            ASTFromInput(scriptCache, "var func63 = function(test63:int) { test63 = 123; };", data);
        }
        
        SUBCASE("function declared in another function with upper variable") {
            ASTFromInput(scriptCache, "var func67 = function(testParam67:int) { var toutou67 = function(blurp:string) { testParam67 = 123; }; testParam67 = 78; };", data);
        }

        SUBCASE("shadowing variable into inner function") {
            ASTFromInput(scriptCache, "var test71 = 3; var func71 = function(test71:string) { test71; };", data);
        }
        
	}

	SUBCASE("Matching failed") {
		DataTestContainer data;
		auto scriptCache = ska::ScriptCacheAST{};
        {
			SUBCASE("Because of unknown symbol") {
				try {
					ASTFromInput(scriptCache, "var i81 = 0; var titi81 = \"llllll\"; { ti81 = 9; }", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

			SUBCASE("Unknown symbol outside when declared as function parameter") {
				try {
					ASTFromInput(scriptCache, "var func90 = function(test90:int) {}; test90 = 123;", data);
				    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
			}

            SUBCASE("used out of function scope") {
                try {
                    ASTFromInput(scriptCache, "var func99 = function(test99:int) { var tout99 = function(blurp99:string) {};}; tout99 = 332;", data);
                    CHECK(false);
                } catch (std::exception& e) {
					CHECK(true);
				}
            }
        }   
		reader = nullptr;
	}
}