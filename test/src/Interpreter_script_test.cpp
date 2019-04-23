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
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "NodeValue/ObjectMemory.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

const auto reservedKeywordsS = ska::ReservedKeywordsPool{};
auto tokenizerS = std::unique_ptr<ska::Tokenizer>{};
std::vector<ska::Token> tokensS;
auto readerIS = std::unique_ptr<ska::Script>{};
auto scriptCacheIS = ska::ScriptCache{};
auto typeCrosserIS = ska::TypeCrosser{};

void ASTFromInputSemanticTCInterpreterScriptNoParse(const std::string& input, DataTestContainer& data) {
    tokenizerS = std::make_unique<ska::Tokenizer>(reservedKeywordsS, input);
    tokensS = tokenizerS->tokenize();
	scriptCacheIS.clear();
	readerIS = std::make_unique<ska::Script>(scriptCacheIS, "main", tokensS);
    
    data.parser = std::make_unique<ska::StatementParser>(reservedKeywordsS);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserIS);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserIS);
	data.interpreter = std::make_unique<ska::Interpreter>(reservedKeywordsS, typeCrosserIS);
}

ska::Script ASTFromInputSemanticTCInterpreterScript(const std::string& input, DataTestContainer& data) {
	ASTFromInputSemanticTCInterpreterScriptNoParse(input, data);
	readerIS->parse(*data.parser);
    return *readerIS;
}

TEST_CASE("[Interpreter Script]") {
	DataTestContainer data;

	SUBCASE("Outside script from file (import)") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Character179 = import \"../test/src/resources/character\";", data);
		auto res = data.interpreter->script(astPtr);
	}

	SUBCASE("Outside script from file (import) and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Character184 = import \"../test/src/resources/character\";var player = Character184.build(\"Player\");var enemy = Character184.default; enemy.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}

	SUBCASE("Outside script from file (import) used by another script, and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser190 = import \"../test/src/resources/character_user\"; CharacterUser190.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}
	
	SUBCASE("Outside script from file (import) used by another script, and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser196 = import \"../test/src/resources/character_user\"; CharacterUser196.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}

	SUBCASE("Custom script starter") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Custom202 = import \"../test/src/resources/custom\"; Custom202.totalAge;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 30);
	}

	SUBCASE("2 outside scripts from file (import)") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser208 = import \"../test/src/resources/character_user\"; CharacterUser208.player.age = 3; var CharacterUser2 = import \"../test/src/resources/character_user\"; CharacterUser2.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 3);
	}

	SUBCASE("2 outside scripts imported with different memory contexts") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Player = import \"../test/src/resources/play\"; var Character = import \"../test/src/resources/character\"; var c = Character.build(\"test\"); Player.run(c);", data);
		data.interpreter->script(astPtr);
	}

	SUBCASE("C++ 1 script-function binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User218 = import \"binding\"; User218.funcTest(14, \"titito\");", data);
		auto test = 0;
		auto testStr = std::string{ "" };
		auto function = std::function<int(int, std::string)>([&](int toto, std::string titi) -> int {
			test = toto;
			testStr = std::move(titi);
			return 0;
		});

		
		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::move(function));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
		CHECK(testStr == "titito");
	}

	SUBCASE("C++ several script-function binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User239 = import \"binding239\"; User239.funcTest(14); User239.funcTest2(\"titito\");", data);
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

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding239", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::move(function1));
		scriptBinding.bindFunction("funcTest2", std::move(function2));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
		CHECK(testStr == "titito");
	}

	SUBCASE("C++ script-function binding generic form, complex object") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User295 = import \"binding295\"; var DataClassImp = import \"dataclass_script\"; var data = DataClassImp.DataClass(\"JeanMi\"); User295.funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.build();

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding295", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.import(*data.parser, { { "DataClassScript", "dataclass_script" } });
		scriptBinding.bindGenericFunction("funcTest", { "DataClassScript::DataClass", "void" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<ska::ObjectMemory>();
			auto* idMap = (*mem)("id").first;
			auto* nameMap = (*mem)("name").first;
			test = idMap->nodeval<int>();
			name = nameMap->nodeval<std::string>();
			return ska::NodeValue{};
		}));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 1234);
		CHECK(name == "JeanMi");
	}

	SUBCASE("C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp = import \"dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp::DataClass) { };"
		"var data = DataClassImp.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
	}

	//TODO move those tests in semantic check side
	//TODO fix ?
	SUBCASE("[fail] C++ -> miniska script-function binding generic : use field defined C++ side") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp = import \"dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp::DataClass) { dummy.id; };"
		"var data = DataClassImp.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.build();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()} == "trying to access to an undeclared field : \"id\" of \"dummy\"");
		}
	}

	SUBCASE("[fail] (bad namespace) C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp = import \"dataclass_script\";"
		"var funcTest = function(dummy: DataClassImpTutu::DataClass) { };"
		"var data = DataClassImp.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.build();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()} == "undeclared custom type \"DataClassImpTutu.DataClass\"");
		}
	}

	SUBCASE("[fail] (bad class name in namespace) C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp = import \"dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp::DataClassImddf) { };"
		"var data = DataClassImp.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.build();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()} == "undeclared custom type \"DataClassImp.DataClassImddf\"");
		}
	}

	SUBCASE("C++ script-function binding with void return") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User264 = import \"binding264\"; User264.funcTest(14);", data);
		auto test = 0;
		
		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding264", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
			test = toto;
		}));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
	}

	SUBCASE("C++ script-function binding generic form") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User279 = import \"binding279\"; User279.funcTest(10);", data);
		auto test = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding279", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindGenericFunction("funcTest", {"int", "void"}, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			test = params[0].nodeval<int>();
			return ska::NodeValue{};
		}));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 10);
	}

	SUBCASE("2 outside scripts from file, with C++ bridging") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var BridgeScript = import \"../test/src/resources/bridge_user\"; var BridgeScript2 = import \"../test/src/resources/bridge_user2\";", data);
		auto test = 0;
		auto count = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
			test += toto;
			count++;
		}));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 3);
		CHECK(count == 2);
	}

	SUBCASE("2 outside scripts from file, with C++ bridging, common script in cache") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var BridgeScript2 = import \"../test/src/resources/bridge_user2\"; var BridgeScript3 = import \"../test/src/resources/bridge_user3\";", data);
		auto test = 0;
		auto count = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(int)>([&](int toto) {
			test += toto;
			count++;
		}));
		scriptBinding.build();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 4);
		CHECK(count == 3);
	}
}
