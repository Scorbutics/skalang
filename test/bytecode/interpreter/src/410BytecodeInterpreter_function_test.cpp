#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] function with 4 parameters (> 3) + use") {
	auto [script, data] = Interpret(
		"test1 = 0-1\n"
		"test2 = 0-1\n"
		"test3 = 0-1\n"
		"test4 = 0-1\n"
		"toto = function(t: int, t1: int, t2: int, t3: int) do\n"
			"test1 = t\n"
			"test2 = t1\n"
			"test3 = t2\n"
			"test4 = t3\n"
		"end\n"
		"toto(1, 2, 3, 4)\n"
		"final = test1: string + test2: string + test3: string + test4: string\n");

	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "1234");
}

TEST_CASE("[BytecodeInterpreter] Custom object creation (field access)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"t = test.test\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 123);
}

TEST_CASE("[BytecodeInterpreter] Custom object creation 2 (field function call)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n"
		"t = test.say(\"titi4\")\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "lol123titi4");
}

// We have to check that calling a function several times with different parameters
// still refers to the same function and the resulting value is different
TEST_CASE("[BytecodeInterpreter] Custom object creation 3 (double field function call)") {
	constexpr auto progStr =
		"toto = function() : var do\n"
			"priv_test = 123\n"
			"return {"
				"test = priv_test\n"
				"say = function(more : string) : string do\n"
					"s = \"lol\" + priv_test + more\n"
					"return s\n"
				"end\n"
			"}\n"
		"end\n"
		"test = toto()\n"
		"test.say(\"titi\")\n"
		"t = test.say(\"titi4\")\n";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "lol123titi4");
}

TEST_CASE("[BytecodeInterpreter] using a function as a parameter") {
	constexpr auto progStr =
		"bi_193 = function() : var do\n"
		"return { test = 14 }\n"
		"end\n"
		"bi_209 = function(toto: bi_193) : bi_193() do\n"
		"return toto()\n"
		"end\n"
		"object = bi_209(bi_193)\n"
		"t = object.test\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 14);
}

TEST_CASE("[BytecodeInterpreter] down scope function variable access") {
	constexpr auto progStr =
		"testValue = 0\n"
		"callback = function() do testValue = 1\n end\n"
		"callback()\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 1);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter") {
	constexpr auto progStr =
		"testValue = 1234\n"
		"callback = function() do testValue = 789\n end\n"
		"lvalFunc219 = function(toto: callback) do\n"
		" toto()\n"
		"end\n"
		"lvalFunc219(callback)\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter without using the source type (function type compatibility)") {
	constexpr auto progStr =
		"lvalFunc218 = function() do end\n"
		"lvalFunc219 = function(toto: lvalFunc218) : lvalFunc218() do\n"
		" toto()\n"
		"end\n"
		"testValue = 1234\n"
		"callback = function() do testValue = 789\n end\n"
		"lvalFunc219(callback)\n"
		"out = testValue\n";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}