#include <doctest.h>
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
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
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
	auto res = interpreted->variable(0);
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
	auto res = interpreted->variable(0);
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
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 123);
}
