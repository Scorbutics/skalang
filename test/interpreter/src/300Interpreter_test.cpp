#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "InterpreterDataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "Runtime/Value/ObjectMemory.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

const auto reservedKeywords = ska::ReservedKeywordsPool{};
auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
std::vector<ska::Token> tokens;
auto readerI = std::unique_ptr<ska::Script>{};
auto scriptCacheI = ska::ScriptCache{};
auto typeCrosserI = ska::TypeCrosser{};

void ASTFromInputSemanticTCInterpreterNoParse(const std::string& input, InterpreterDataTestContainer& data) {
	tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
	tokens = tokenizer->tokenize();
	scriptCacheI.cache.clear();
	scriptCacheI.astCache.clear();
	readerI = std::make_unique<ska::Script>(scriptCacheI, "main", tokens);
	
	data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.interpreter = std::make_unique<ska::Interpreter>(reservedKeywords, typeCrosserI);
}

ska::Script ASTFromInputSemanticTCInterpreter(const std::string& input, InterpreterDataTestContainer& data) {
	ASTFromInputSemanticTCInterpreterNoParse(input, data);
	readerI->astScript().parse(*data.parser);
	return *readerI;
}

TEST_CASE("[Interpreter]") {
	InterpreterDataTestContainer data;
	
	SUBCASE("OK") {
		SUBCASE("Basic Maths") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("(4 * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.interpreter->script(astPtr);
	CHECK(res.nodeval<long>() == 42);
		}
		SUBCASE("Basic Maths with var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 42);
		}

		SUBCASE("Var declared") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 14; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 14);
		}

		SUBCASE("Var reaffected") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = 25;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 25);
		}

		SUBCASE("Var reaffected using the same var (int)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = toto * 2;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 8);
		}

		SUBCASE("Var reaffected using the same var (string)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\"; toto; toto = toto * 2;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK((*res.nodeval<ska::StringShared>() == "titi"));
		}

		SUBCASE("Var reaffected string") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\" * 2 + \"to\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK((*res.nodeval<ska::StringShared>() == "titito"));
		}

		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK((*res.nodeval<ska::StringShared>() == "2ti"));
		}

		SUBCASE("Var reaffected string with number * 2") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\" * 2; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK((*res.nodeval<ska::StringShared>() == "2titi"));
		}

		SUBCASE("For loop statement") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var tititi = 2; for(var i = 0; i < 5; i = i + 1) { tititi = tititi + i; } tititi; ", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 12);
		}

		SUBCASE("If block true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 123);
		}

		SUBCASE("If block false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 1);
		}

		SUBCASE("If block pure true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 0; if(true) { toto = 123; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 123);
		}

		SUBCASE("If Else block if false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 24);
		}

		SUBCASE("If Else block if true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 123);
		}

		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK((*res.nodeval<ska::StringShared>() == "2ti"));
		}

		SUBCASE("Array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 25);
		}

		SUBCASE("Array of int : add a cell") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25]; toto = toto + 4;", data);
			auto res = data.interpreter->script(astPtr).nodeval<ska::NodeValueArray>();
			CHECK(res->size() == 3);
			CHECK((*res)[0] == 14L);
			CHECK((*res)[1] == 25L);
			CHECK((*res)[2] == 4L);
		}

		SUBCASE("Array of int : del a cell") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto = toto - 2;", data);
			auto res = data.interpreter->script(astPtr).nodeval<ska::NodeValueArray>();
			CHECK(res->size() == 2);
			CHECK((*res)[0] == 14L);
			CHECK((*res)[1] == 25L);
		}

		SUBCASE("Array of int : del a range of cells") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto = toto - [1, 2];", data);
			auto res = data.interpreter->script(astPtr).nodeval<ska::NodeValueArray>();
			CHECK(res->size() == 2);
			CHECK((*res)[0] == 14L);
			CHECK((*res)[1] == 2L);
		}

		SUBCASE("Assigning a cell of an array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1] = 226; toto[1];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 226);
		}

		SUBCASE("Basic function") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function() : int { return 3; }; var titi = totoFunc(); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 3);
		}
		
		SUBCASE("Function 1 parameter") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 10);
		}

		SUBCASE("Function 1 parameter 2x calls") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi = totoFunc(17);", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 20);
		}

		SUBCASE("Function 1 parameter using a parent scope var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var test = 20; var totoFunc = function(value: int) : int { return test + value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 30);
		}

		SUBCASE("Function 0 parameter creating custom object") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function() : var { return { num: 3, test: \"test\" }; }; var titi = totoFunc(); titi.num;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 3);
		}
			
		SUBCASE("Function 0 parameter creating custom object with use as parameter of another one") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Dummy = function(): var { return { data: 3 }; }; var Runner = function(): var { var print = function(i: Dummy()): int { return i.data; }; return { print : print }; }; Runner().print(Dummy());", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 3);
		}

		SUBCASE("Array use from field access") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Coldragon = function() : var{ var array = [20, 150, 2]; return { array : array }; }; var target = Coldragon(); var data = target.array[0] - 88; data;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == -68);
		}

		SUBCASE("double array string : cell") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var str167 = [[0, 1], [2, 3]]; str167[0]; str167[0][1];", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 1);
		}

		SUBCASE("using a function as a parameter") {
			auto astPtr = ASTFromInputSemanticTCInterpreter(
				"var lvalFunc206 = function() : var {"
				"return { test : 14 };"
				"};"
				"var lvalFunc209 = function(toto: lvalFunc206) : lvalFunc206() {"
				"return toto();"
				"};"
				"var object = lvalFunc209(lvalFunc206);"
				"object.test;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 14);
		}

		SUBCASE("using a callback function as a parameter without using the source type (function type compatibility)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter(
				"var lvalFunc218 = function() {};"
				"var lvalFunc219 = function(toto: lvalFunc218) : lvalFunc218() {"
				"	toto();"
				"};"
				"var testOut = 1234;"
				"var callback = function() { testOut = 789; };"
				"lvalFunc219(callback);", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<long>() == 789);
		}

		SUBCASE("for with empty statement in if") {
			auto astPtr = ASTFromInputSemanticTCInterpreter(
				"var size = function() : int { return 10;};"
				"if (size() > 0) {"
					"for(var i = 0; i < size(); i = i + 1);"
				"}", data);
			data.interpreter->script(astPtr);
		}

		SUBCASE("Fail") {
			SUBCASE("Array : del cells : failure out of bound") {
				try {
					auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14.0, 25.0, 13.0, 2.0]; toto = toto - 12;", data);
					auto res = data.interpreter->script(astPtr).nodeval<ska::NodeValueArray>();
					CHECK(false);
				} catch (std::exception & e) {
					CHECK(e.what() == std::string{ "Bad vector-based index deletion : out of bound" });
				}
			}
			
			SUBCASE("Array : del a range of cell : failure out of bound") {
				try {
					auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14.0, 25.0, 13.0, 2.0]; toto = toto - [1, 12];", data);
					auto res = data.interpreter->script(astPtr).nodeval<ska::NodeValueArray>();
					CHECK(false);
				} catch (std::exception & e) {
					CHECK(e.what() == std::string{ "Bad vector-based index deletion : out of bound" });
				}
			}
		}
	}
		
}
