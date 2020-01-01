#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] if : if body") {
	static constexpr auto progStr =
	"var t = false;"
	"if ([18] == [18]) {"
	"t = true;"
	"}";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] if : else body") {
	static constexpr auto progStr =
	"var t = 0;"
	"if ([18] == [1124]) {"
	"t = 1;"
	"} else {"
	"t = 2;"
	"}";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 2);
}

TEST_CASE("[BytecodeInterpreter] for without body") {
	static constexpr auto progStr = "for(var i = 0; i < 10; i = i + 1);";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] for with body") {
	static constexpr auto progStr = "var toto = 123; for(var i = 0; i < 10; i = i + 1) { toto = toto + i * 2; } var test = toto;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 213);
}
