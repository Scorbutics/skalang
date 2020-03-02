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

	auto astPtr = ASTFromInput(scriptCache, "i = 0\n titi = \"llllll\"\n do toto = 2\n i = 9\n end", data);
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
	CHECK(i == nestedI);
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
			auto astPtr = ASTFromInput(scriptCache, "i = 0\n i = 123\n do i = 9\n end", data);
			auto& table = reader->symbols();
			
			CHECK(table.nested().size() == 1);
			auto nestedI = (*table.nested()[0])["i"];
			auto i = table["i"];

			CHECK(i != nullptr);
		}

		SUBCASE("var in upper scope used into inner function scope") {
			auto astPtr = ASTFromInput(scriptCache, "test59 = 21\n func59 = function() do test59 = 123\n end\n", data);
			auto& table = reader->symbols();

			CHECK(table.nested().size() == 1);
			auto& functionTable = (*table.nested()[0]);
			auto nestedVar = functionTable["test59"];
			auto var = table["test59"];

			CHECK(nestedVar == var);
		}

		SUBCASE("function parameter use into function") {
			auto astPtr = ASTFromInput(scriptCache, "func63 = function(test63:int) do test63 = 123\n end\n", data);
			auto& table = reader->symbols();

			CHECK(table.nested().size() == 1);
			auto& functionTable = (*table.nested()[0]);
			auto nestedVar = functionTable["test63"];
			auto var = table["test63"];

			CHECK(var == nullptr);
			CHECK(nestedVar != nullptr);
		}
	
		SUBCASE("function declared in another function with upper variable") {
			ASTFromInput(scriptCache, "func67 = function(testParam67:int) do toutou67 = function(blurp:string) do testParam67 = 123\n end\n testParam67 = 78\n end\n", data);
		}

		SUBCASE("shadowing variable into inner function") {
			ASTFromInput(scriptCache, "test71 = 3\n func71 = function(test71:string) do test71\n end\n", data);
		}
	
	}

}
