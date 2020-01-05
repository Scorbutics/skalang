#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorTest.h"

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] import ") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var Player = import \"" SKALANG_TEST_DIR "/src/resources/play\";");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	BytecodeCompare(res, {
		{ Command::SCRIPT, "R0", "1" },
		{ Command::MOV, "V0", "R0" }
	});
}

TEST_CASE("[BytecodeGenerator] Outside script from file (import) and use") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(
		"var Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var player = Character184.build(\"Player\");"
		"var enemy = Character184.default; enemy.age;");
	auto& res = data.generator->generate(*data.storage, std::move(astPtr));
	//CHECK(res.nodeval<int>() == 10);
}

TEST_CASE("[BytecodeGenerator] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"var Character91 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character92 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character91.default.age = 123;"
		"var t = Character92.default.age;";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(astPtr));
}

TEST_CASE("[BytecodeGenerator] C++ 1 script-function binding") {
	/*auto [astPtr, data] = ASTFromInputBytecodeGenerator("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto& res = data.generator->generate(std::move(astPtr));

	BytecodeCompare(res, {
	});*/

	/*
	auto data = ASTFromInputBytecodeGeneratorNoParse("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto test = 0;
	auto testStr = std::string{ "" };
	auto function = std::function<int(ska::Script&, int, ska::StringShared)>(
		[&](ska::Script&, int toto, ska::StringShared titi) -> int {
		test = toto;
		testStr = std::move(*titi);
		return 0;
	});


	auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
	scriptBinding.bindFunction("funcTest", std::move(function));
	scriptBinding.buildFunctions();

	readerI->parse(*data.parser);
	auto generated = data.generator->generate(ska::bytecode::Script{ *readerI });
	*/
}

