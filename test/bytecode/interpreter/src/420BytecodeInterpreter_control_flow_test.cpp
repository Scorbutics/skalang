#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] if : if body") {
	static constexpr auto progStr =
	"t = false\n"
	"if ([18] == [18]) do "
	"t = true "
	"end";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] if : else body") {
	static constexpr auto progStr =
	"t = 0\n"
	"if ([18] == [1124]) do\n"
	"t = 1\n"
	"end else do\n"
	"t = 2 "
	"end\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 2);
}

TEST_CASE("[BytecodeInterpreter] for without body") {
	static constexpr auto progStr = "for(i = 0\n i < 10\n i = i + 1) do end\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] for with body") {
	static constexpr auto progStr = "toto = 123\n for(i = 0\n i < 10\n i = i + 1) do toto = toto + i * 2\n end test = toto\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 213);
}
