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

	SUBCASE("miniska -> C++ binding : using a miniska field from a C++ -> miniska binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
			"var DataClassImp280 = import \"bind:dataclass_script\";"
			"var runner = DataClassImp280.run();"
			"runner.getToto();", data);

		auto scriptEmBinding = ska::ScriptBridge{ scriptCacheIS, "binding1_lib", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptEmBinding.bindFunction("getToto", std::function<std::string(ska::Script&)>([](ska::Script&) {
			return "tototo !";
		}));
		scriptEmBinding.buildFunctions();

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.import(*data.parser, *data.interpreter, { {"Test293", "../../../test/src/resources/test293"}, {"Binding1", "bind:binding1_lib"} });
		scriptBindingDataClass.bindGenericFunction("run", { "Test293::Fcty()" }, std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script& caller, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto getTotoMemory = scriptBindingDataClass.accessMemory("Binding1", "getToto");
			assert(getTotoMemory.first != nullptr);
			auto mem = scriptBindingDataClass.createMemory();
			mem->emplace("getToto", getTotoMemory.first->clone());
			return ska::NodeValue{ std::move(mem) };
		}));
		scriptBindingDataClass.buildFunctions();

		readerIS->parse(*data.parser);
		auto result = data.interpreter->script(*readerIS);
		CHECK(result.nodeval<std::string>() == "tototo !");
	}

	SUBCASE("Outside script from file (import)") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Character179 = import \"../../test/src/resources/character\";", data);
		auto res = data.interpreter->script(astPtr);
	}

	SUBCASE("Outside script from file (import) and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Character184 = import \"../../test/src/resources/character\";var player = Character184.build(\"Player\");var enemy = Character184.default; enemy.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}

	SUBCASE("Outside script from file (import) used by another script, and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser190 = import \"../../test/src/resources/character_user\"; CharacterUser190.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}
	
	SUBCASE("Outside script from file (import) used by another script, and use") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser196 = import \"../../test/src/resources/character_user\"; CharacterUser196.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 10);
	}

	SUBCASE("Custom script starter") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Custom202 = import \"../../test/src/resources/custom\"; Custom202.totalAge;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 30);
	}

	SUBCASE("2 outside scripts from file (import)") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var CharacterUser208 = import \"../../test/src/resources/character_user\"; CharacterUser208.player.age = 3; var CharacterUser2 = import \"../../test/src/resources/character_user\"; CharacterUser2.player.age;", data);
		auto res = data.interpreter->script(astPtr);
		CHECK(res.nodeval<int>() == 3);
	}

	SUBCASE("2 outside scripts imported with different memory contexts") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Player = import \"../../test/src/resources/play\"; var Character = import \"../../test/src/resources/character\"; var c = Character.build(\"test\"); Player.run(c);", data);
		data.interpreter->script(astPtr);
	}

	SUBCASE("C++ 1 script-function binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User218 = import \"bind:binding\"; User218.funcTest(14, \"titito\");", data);
		auto test = 0;
		auto testStr = std::string{ "" };
		auto function = std::function<int(ska::Script&, int, std::string)>([&](ska::Script&, int toto, std::string titi) -> int {
			test = toto;
			testStr = std::move(titi);
			return 0;
		});

		
		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::move(function));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
		CHECK(testStr == "titito");
	}

	SUBCASE("C++ several script-function binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User239 = import \"bind:binding239\"; User239.funcTest(14); User239.funcTest2(\"titito\");", data);
		auto test = 0;
		auto testStr = std::string{ "" };

		auto function1 = std::function<int(ska::Script&, int)>([&](ska::Script&, int toto) -> int {
			test = toto;
			return 0;
		});
		auto function2 = std::function<int(ska::Script&, std::string)>([&](ska::Script&, std::string titi) -> int {
			testStr = std::move(titi);
			return 0;
		});

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding239", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::move(function1));
		scriptBinding.bindFunction("funcTest2", std::move(function2));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
		CHECK(testStr == "titito");
	}

	SUBCASE("C++ script-function binding generic form, complex object") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User295 = import \"bind:binding295\"; var DataClassImp = import \"bind:dataclass_script\"; var data = DataClassImp.DataClass(\"JeanMi\"); User295.funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.buildFunctions();

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding295", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.import(*data.parser, *data.interpreter, { { "DataClassScript", "bind:dataclass_script" } });
		scriptBinding.bindGenericFunction("funcTest", { "DataClassScript::DataClass()", "void" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<ska::ObjectMemory>();
			auto* idMap = (*mem)("id").first;
			auto* nameMap = (*mem)("name").first;
			test = idMap->nodeval<int>();
			name = nameMap->nodeval<std::string>();
			return ska::NodeValue{};
		}));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 1234);
		CHECK(name == "JeanMi");
	}

	SUBCASE("C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp169 = import \"bind:dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp169::DataClass()) { };"
		"var data = DataClassImp169.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
	}

	//TODO move those tests in semantic check side
	//TODO fix ?
	SUBCASE("[fail] C++ -> miniska script-function binding generic : use field defined C++ side") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp194 = import \"bind:dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp194::DataClass()) { dummy.id; };"
		"var data = DataClassImp194.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.buildFunctions();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("trying to access to an undeclared field : \"id\" of \"dummy\"") != std::string::npos);
		}
	}

	SUBCASE("[fail] (bad namespace) C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp222 = import \"bind:dataclass_script\";"
		"var funcTest = function(dummy: DataClassImpTutu222::DataClass()) { };"
		"var data = DataClassImp222.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.buildFunctions();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("undeclared custom type \"DataClassImpTutu222::DataClass\" (when trying to look on token type \"void\")") != std::string::npos);
		}
	}

	SUBCASE("[fail] (bad class name in namespace) C++ -> miniska script-function binding generic") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
		"var DataClassImp250 = import \"bind:dataclass_script\";"
		"var funcTest = function(dummy: DataClassImp250::DataClassImddf()) { };"
		"var data = DataClassImp250.DataClass(\"JeanMi\");"
		"funcTest(data);", data);
		auto test = 0;
		std::string name;

		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.bindGenericFunction("DataClass", { "string", "var" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto mem = params[0].nodeval<std::string>();
			auto result = ska::MemoryTable::create();
			result->emplace("id", 1234);
			result->emplace("name", std::move(mem));
			return ska::NodeValue{ std::move(result) };
		}));
		scriptBindingDataClass.buildFunctions();

		try {
			readerIS->parse(*data.parser);
			data.interpreter->script(*readerIS);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("undeclared custom type \"DataClassImp250::DataClassImddf\" (when trying to look on token type \"var C:\\DevFast\\skalang\\bin\\Debug\\std\\dataclass_script.miniska\")") != std::string::npos);
		}
	}

	SUBCASE("miniska -> C++ binding : calling a miniska function from a C++ -> miniska binding") {
		ASTFromInputSemanticTCInterpreterScriptNoParse(
			"var DataClassImp306 = import \"bind:dataclass_script\";"
			"DataClassImp306.run().name;", data);


		auto scriptBindingDataClass = ska::ScriptBridge{ scriptCacheIS, "dataclass_script", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBindingDataClass.import(*data.parser, *data.interpreter, { {"Character", "../../../test/src/resources/character"} });
		scriptBindingDataClass.bindGenericFunction("run", { "Character::build()" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> p) -> ska::NodeValue {
			auto params = std::vector<ska::NodeValue>{};
			params.push_back(std::string{ "babar" });
			auto character = scriptBindingDataClass.callFunction(*data.interpreter, "Character", "build", std::move(params));
			auto& memCharacter = character.nodeval<ska::ObjectMemory>();
			memCharacter->emplace("name", std::string{ "titito" });
			return ska::NodeValue{ std::move(memCharacter) };
		}));
		scriptBindingDataClass.buildFunctions();

		readerIS->parse(*data.parser);
		auto result = data.interpreter->script(*readerIS);
		CHECK(result.nodeval<std::string>() == "titito");
	}

	SUBCASE("using a callback in another script & another context") {
		auto astPtr = ASTFromInputSemanticTCInterpreterScript("var Script317 = import \"../../test/src/resources/test317_1\"; Script317.actualCharacter.name;", data);
		auto result = data.interpreter->script(astPtr);
		CHECK(result.nodeval<std::string>() == "test317");
	}

	SUBCASE("C++ script-function binding with void return") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User264 = import \"bind:binding264\"; User264.funcTest(14);", data);
		auto test = 0;
		
		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding264", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(ska::Script&, int)>([&](ska::Script&, int toto) {
			test = toto;
		}));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 14);
	}

	SUBCASE("C++ script-function binding generic form") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var User279 = import \"bind:binding279\"; User279.funcTest(10);", data);
		auto test = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding279", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindGenericFunction("funcTest", {"int", "void"}, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			test = params[0].nodeval<int>();
			return ska::NodeValue{};
		}));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 10);
	}

	SUBCASE("2 outside scripts from file, with C++ bridging") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var BridgeScript = import \"../../test/src/resources/bridge_user\"; var BridgeScript2 = import \"../../test/src/resources/bridge_user2\";", data);
		auto test = 0;
		auto count = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(ska::Script&, int)>([&](ska::Script&, int toto) {
			test += toto;
			count++;
		}));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 3);
		CHECK(count == 2);
	}

	SUBCASE("2 outside scripts from file, with C++ bridging, common script in cache") {
		ASTFromInputSemanticTCInterpreterScriptNoParse("var BridgeScript2 = import \"../../test/src/resources/bridge_user2\"; var BridgeScript3 = import \"../../test/src/resources/bridge_user3\";", data);
		auto test = 0;
		auto count = 0;

		auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding322", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
		scriptBinding.bindFunction("funcTest", std::function<void(ska::Script&, int)>([&](ska::Script&, int toto) {
			test += toto;
			count++;
		}));
		scriptBinding.buildFunctions();

		readerIS->parse(*data.parser);
		data.interpreter->script(*readerIS);
		CHECK(test == 4);
		CHECK(count == 3);
	}
}
