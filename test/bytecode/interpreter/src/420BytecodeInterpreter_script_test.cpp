#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) and use") {
	constexpr auto progStr = 
		"var Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var player = Character184.build(\"Player\");"
		"var enemy = Character184.default;"
		"var t = enemy.age;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) - edit - and use") {
	constexpr auto progStr = 
		"var Character260 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var enemy = Character260.default;"
		"enemy.age = 99;"
		"var t = enemy.age;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 99);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script : ensure we do not try to recompile neither rerun it") {
	constexpr auto progStr =
		"var Character270 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character272 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character270.default.age;"
		"var t = Character272.default.age;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"var Character284 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character285 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character284.default.age = 123;"
		"var t = Character285.default.age;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 123);
}
