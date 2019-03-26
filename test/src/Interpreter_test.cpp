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
#include "Service/ScriptBinding.h"
#include "Service/Script.h"
#include "Service/ScriptCache.h"
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

		SUBCASE("Outside script from file (import)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Character179 = import \"../test/src/resources/character\";", data);
			auto res = data.interpreter->script(astPtr);
		}

		SUBCASE("Outside script from file (import) and use") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Character184 = import \"../test/src/resources/character\";var player = Character184.build(\"Player\");var enemy = Character184.default; enemy.age;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 10);
		}

		SUBCASE("Outside script from file (import) used by another script, and use") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var CharacterUser190 = import \"../test/src/resources/character_user\"; CharacterUser190.player.age;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 10);
		}
		
		SUBCASE("Outside script from file (import) used by another script, and use") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var CharacterUser196 = import \"../test/src/resources/character_user\"; CharacterUser196.player.age;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 10);
		}

		SUBCASE("Custom script starter") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var Custom202 = import \"../test/src/resources/custom\"; Custom202.totalAge;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 30);
		}

		SUBCASE("2 outside scripts from file (import)") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var CharacterUser208 = import \"../test/src/resources/character_user\"; CharacterUser208.player.age = 3; var CharacterUser2 = import \"../test/src/resources/character_user\"; CharacterUser2.player.age;", data);
			auto res = data.interpreter->script(astPtr);
			CHECK(res.nodeval<int>() == 3);
		}

        SUBCASE("C++ 1 script-function binding") {
			ASTFromInputSemanticTCInterpreterNoParse("var User218 = import \"binding\"; User218.funcTest(14, \"titito\");", data);
			auto test = 0;
			auto testStr = std::string{ "" };
			auto function = std::function<int(int, std::string)>([&](int toto, std::string titi) -> int {
				test = toto;
				testStr = std::move(titi);
				return 0;
			});

			
			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindFunction("funcTest", std::move(function));
			scriptBinding.build();

            readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 14);
			CHECK(testStr == "titito");
		}

        SUBCASE("C++ several script-function binding") {
			ASTFromInputSemanticTCInterpreterNoParse("var User239 = import \"binding239\"; User239.funcTest(14); User239.funcTest2(\"titito\");", data);
			auto test = 0;
			auto testStr = std::string{ "" };

			auto function1 = std::function<int(int)>([&](int toto) -> int {
				test = toto;
				return 0;
			});
			auto function2 = std::function<int(std::string)>([&](std::string titi) -> int {
				testStr = std::move(titi);
				return 0;
			});

			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding239", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindFunction("funcTest", std::move(function1));
			scriptBinding.bindFunction("funcTest2", std::move(function2));
			scriptBinding.build();

            readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 14);
			CHECK(testStr == "titito");
		}

        SUBCASE("C++ script-function binding with void return") {
			ASTFromInputSemanticTCInterpreterNoParse("var User264 = import \"binding264\"; User264.funcTest(14);", data);
			auto test = 0;
			
			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding264", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
				test = toto;
			}));
			scriptBinding.build();

            readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 14);
		}

		SUBCASE("C++ script-function binding generic form") {
			ASTFromInputSemanticTCInterpreterNoParse("var User279 = import \"binding279\"; User279.funcTest(10);", data);
			auto test = 0;

			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding279", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindGenericFunction("funcTest", {"int", "void"}, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
				test = params[0].nodeval<int>();
				return ska::NodeValue{};
			}));
			scriptBinding.build();

			readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 10);
		}

		SUBCASE("C++ script-function binding generic form, complex object") {
			ASTFromInputSemanticTCInterpreterNoParse("var User295 = import \"binding295\"; var DataClass = function(name: string) : var { return { id : 1234, name : name }; }; var data = DataClass(\"JeanMi\"); User295.funcTest(data);", data);
			auto test = 0;
			std::string name;

			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding295", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindGenericFunction("funcTest", { "var", "void" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
				auto mem = params[0].nodeval<ska::ObjectMemory>();
				auto* idMap = (*mem)("id").first;
				auto* nameMap = (*mem)("name").first;
				test = idMap->nodeval<int>();
				name = nameMap->nodeval<std::string>();
				return ska::NodeValue{};
			}));
			scriptBinding.build();

			readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 1234);
			CHECK(name == "JeanMi");
		}

		SUBCASE("2 outside scripts from file, with C++ bridging") {
			ASTFromInputSemanticTCInterpreterNoParse("var BridgeScript = import \"../test/src/resources/bridge_user\"; var BridgeScript2 = import \"../test/src/resources/bridge_user2\";", data);
			auto test = 0;
			auto count = 0;

			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
				test += toto;
				count++;
			}));
			scriptBinding.build();

			readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 3);
			CHECK(count == 2);
		}

		SUBCASE("2 outside scripts from file, with C++ bridging, common script in cache") {
			ASTFromInputSemanticTCInterpreterNoParse("var BridgeScript2 = import \"../test/src/resources/bridge_user2\"; var BridgeScript3 = import \"../test/src/resources/bridge_user3\";", data);
			auto test = 0;
			auto count = 0;

			auto scriptBinding = ska::ScriptBridge{ scriptCacheI, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywords };
			scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
				test += toto;
				count++;
			}));
			scriptBinding.build();

			readerI->parse(*data.parser);
			data.interpreter->script(*readerI);
			CHECK(test == 4);
			CHECK(count == 3);
		}

	}
		
}
