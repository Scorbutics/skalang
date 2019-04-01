#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "NodeValue/ObjectMemory.h"

const auto reservedKeywords = ska::ReservedKeywordsPool{};
auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
std::vector<ska::Token> tokens;
auto readerI = std::unique_ptr<ska::Script>{};
auto scriptCacheI = ska::ScriptCache{};

void ASTFromInputSemanticTCInterpreterNoParse(const std::string& input, DataTestContainer& data) {
    tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
    tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::Script>(scriptCacheI, "main", tokens);
    
    data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser);
	data.interpreter = std::make_unique<ska::Interpreter>(reservedKeywords);
}

ska::Script ASTFromInputSemanticTCInterpreter(const std::string& input, DataTestContainer& data) {
	ASTFromInputSemanticTCInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
    return *readerI;
}

TEST_CASE("[Interpreter]") {
    DataTestContainer data;
    
	SUBCASE("OK") {
		SUBCASE("Basic Maths") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("(4 * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.interpreter->script(astPtr);
            CHECK(res.nodeval<int>() == 42);
		}
		SUBCASE("Basic Maths with var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 42);
		}

		SUBCASE("Var declared") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 14; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 14);
		}

		SUBCASE("Var reaffected") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = 25;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 25);
		}

		SUBCASE("Var reaffected using the same var (int)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = toto * 2;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 8);
		}

		SUBCASE("Var reaffected using the same var (string)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\"; toto; toto = toto * 2;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<std::string>() == "titi");
		}

		SUBCASE("Var reaffected string") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\" * 2 + \"to\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<std::string>() == "titito");
		}
		
		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<std::string>() == "2ti");
		}

		SUBCASE("Var reaffected string with number * 2") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\" * 2; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<std::string>() == "2titi");
		}

		SUBCASE("For loop statement") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var tititi = 2; for(var i = 0; i < 5; i = i + 1) { tititi = tititi + i; } tititi; ", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 12);
		}

        SUBCASE("If block true") {
            auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 123);
        }

		SUBCASE("If block false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 1);
		}

		SUBCASE("If block pure true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 0; if(true) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 123);
		}

		SUBCASE("If Else block if false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 24);
		}

		SUBCASE("If Else block if true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 123);
		}

		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<std::string>() == "2ti");
		}

		SUBCASE("Array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 25);
		}

		SUBCASE("Array of int : add a cell") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25]; toto = toto + 4; toto[2];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 4);
		}

		SUBCASE("Assigning a cell of an array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1] = 226; toto[1];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 226);
		}

		SUBCASE("Basic function") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function() : int { return 3; }; var titi = totoFunc(); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 3);
		}
		
		SUBCASE("Function 1 parameter") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 10);
		}

		SUBCASE("Function 1 parameter 2x calls") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi = totoFunc(17);", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 20);
		}

		SUBCASE("Function 1 parameter using a parent scope var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var test = 20; var totoFunc = function(value: int) : int { return test + value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 30);
		}

		SUBCASE("Function 0 parameter creating custom object") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function() : var { return { num: 3, test: \"test\" }; }; var titi = totoFunc(); titi.num;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 3);
		}
			
		SUBCASE("Function 0 parameter creating custom object with use as parameter of another one") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Dummy = function(): var { return { data: 3 }; }; var Runner = function(): var { var print = function(i: Dummy): int { return i.data; }; return { print : print }; }; Runner().print(Dummy());", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 3);
		}

		SUBCASE("Array use from field access") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Coldragon = function() : var{ var array = [20, 150, 2]; return { array : array }; }; var target = Coldragon(); var data = target.array[0] - 88; data;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == -68);
		}
	}
		
}
