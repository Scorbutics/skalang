#include <iostream>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterTest.h"

TEST_CASE("[BytecodeInterpreter] literal alone") {
	auto [script, data] = Interpret("4;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.id(), data.storage);
}

TEST_CASE("[BytecodeInterpreter] var declaration") {
	auto [script, data] = Interpret("var toto = 4;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] var declaration from var") {
	auto [script, data] = Interpret("var toto = 4; var titi = toto;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths linear") {
	auto [script, data] = Interpret("var t = 3 + 4 - 1;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 6);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 left subpart") {
	auto [script, data] = Interpret("var t = (3 + 4) * 2;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 right subpart") {
	auto [script, data] = Interpret("var t = 2 * (3 + 4);");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths subparts") {
	auto [script, data] = Interpret("var t = (3 + 4) * (1 + 2);");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 21);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths with var") {
	auto [script, data] = Interpret("var toto = 4; var t = (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 44);
}

TEST_CASE("[BytecodeInterpreter] var expression declaration") {
  auto [script, data] = Interpret("var result = 7 + 3;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] Introducing block sub-variable") {
	auto [script, data] = Interpret("var toto = 4; { var toto = 5; toto + 1; } var t = toto + 1;");
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 5);
}
