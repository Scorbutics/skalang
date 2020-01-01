#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] Custom object creation (field access)") {
	constexpr auto progStr =
		"var toto = function() : var {"
			"var priv_test = 123;"
			"return {"
				"test : priv_test,"
				"say : function(more : string) : string {"
					"var s = \"lol\" + priv_test + more;"
					"return s;"
				"}"
			"};"
		"};"
		"var test = toto();"
		"var t = test.test;";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 123);
}

TEST_CASE("[BytecodeInterpreter] Custom object creation 2 (field function call)") {
	constexpr auto progStr =
		"var toto = function() : var {"
			"var priv_test = 123;"
			"return {"
				"test : priv_test,"
				"say : function(more : string) : string {"
					"var s = \"lol\" + priv_test + more;"
					"return s;"
				"}"
			"};"
		"};"
		"var test = toto();"
		"test.say(\"titi\");"
		"var t = test.say(\"titi4\");";

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
		"var toto = function() : var {"
			"var priv_test = 123;"
			"return {"
				"test : priv_test,"
				"say : function(more : string) : string {"
					"var s = \"lol\" + priv_test + more;"
					"return s;"
				"}"
			"};"
		"};"
		"var test = toto();"
		"test.say(\"titi\");"
		"var t = test.say(\"titi4\");";

	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "lol123titi4");
}

TEST_CASE("[BytecodeInterpreter] using a function as a parameter") {
	constexpr auto progStr =
		"var bi_193 = function() : var {"
		"return { test : 14 };"
		"};"
		"var bi_209 = function(toto: bi_193) : bi_193() {"
		"return toto();"
		"};"
		"var object = bi_209(bi_193);"
		"var t = object.test;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 14);
}

TEST_CASE("[BytecodeInterpreter] down scope function variable access") {
	constexpr auto progStr =
		"var testValue = 0;"
		"var callback = function() { testValue = 1; };"
		"callback();"
		"var out = testValue;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 1);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter") {
	constexpr auto progStr =
		"var testValue = 1234;"
		"var callback = function() { testValue = 789; };"
		"var lvalFunc219 = function(toto: callback) {"
		" toto();"
		"};"
		"lvalFunc219(callback);"
		"var out = testValue;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}

TEST_CASE("[BytecodeInterpreter] using a callback function as a parameter without using the source type (function type compatibility)") {
	constexpr auto progStr =
		"var lvalFunc218 = function() {};"
		"var lvalFunc219 = function(toto: lvalFunc218) : lvalFunc218() {"
		" toto();"
		"};"
		"var testValue = 1234;"
		"var callback = function() { testValue = 789; };"
		"lvalFunc219(callback);"
		"var out = testValue;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), *data.storage)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}