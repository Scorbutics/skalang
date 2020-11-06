#include <doctest.h>
#include <iostream>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) and use") {
	constexpr auto progStr =
		"Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"player = Character184.build(\"Player\")\n"
		"enemy = Character184.default\n"
		"t = enemy.age\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));

	ska::bytecode::InstructionsDebugInfo{ progStr, 50 }.print(std::cout, *data.storage, gen.id());

	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());

	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) - edit - and use") {
	constexpr auto progStr = 
		"Character260 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"enemy = Character260.default\n"
		"enemy.age = 99\n"
		"t = enemy.age\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 99);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script : ensure we do not try to recompile neither rerun it") {
	constexpr auto progStr =
		"Character270 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character272 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character270.default.age\n"
		"t = Character272.default.age\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"Character284 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character285 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"Character284.default.age = 123\n"
		"t = Character285.default.age\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 123);
}


TEST_CASE("[BytecodeInterpreter] Complex use of factory instances inside itself") {
	constexpr auto progStr =
	"JSON = function(v : string) : var do\n"
		"child = [] : JSON()\n"
		"return { \n"
			"Size = function() : int do\n"
				"return child.size()\n"
			"end\n"
			"Get = function(key : int) : JSON() do\n"
				"return child[key]\n"
			"end\n"
			"Add = function(c : JSON()) do\n"
				"child = child + c\n"
			"end\n"
			"Value = function() : string do\n"
				"return v\n"
			"end\n"
		"}\n"
	"end\n"

	"json = JSON(\"toto\")\n"
	"json.Add(JSON(\"titi\"))\n"

	"out = json.Get(0).Value() + json.Value()\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(gen.id());
	auto cellValue = *res.nodeval<ska::StringShared>();
	CHECK(cellValue == "tititoto");
}
