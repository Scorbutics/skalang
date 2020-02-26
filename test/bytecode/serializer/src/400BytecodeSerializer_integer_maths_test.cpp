#include <iostream>
#include <iomanip>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeSerializerTest.h"

static std::stringstream SerializingStream = std::stringstream{};

ska::bytecode::SerializationStrategy SerializeInStream() {
	return [](const std::string& scriptName) -> std::ostream& {
		std::cout << "serializing " << scriptName << std::endl;
		return SerializingStream;
	};
}

ska::bytecode::DeserializationStrategy DeserializeInStream() {
	return [](const std::string& scriptName) -> std::istream& {
		std::cout << "deserializing " << scriptName << std::endl;
		return SerializingStream;
	};
}

void DisplayStringStream() {
	uint8_t buf = { };
	auto copy = std::stringstream{};
	copy << SerializingStream.str();
	do {
		copy.read((char*)&buf, sizeof(buf));
		if (!copy.fail()) {
			std::cout << std::setfill('0') << std::setw(2) << std::hex << int(buf)<< " ";
		}
	} while (!copy.fail());
	std::cout << std::endl;
}

TEST_CASE("[BytecodeSerializer] literal alone") {
	auto [script, data] = Serialize("4;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream(), true);
	CHECK(original.empty());

	// A way to test instructions emptyness
	CHECK(destinationCache.at(0).begin() == destinationCache.at(0).end());
}

TEST_CASE("[BytecodeSerializer] var declaration") {
	auto [script, data] = Serialize("var toto = 4;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream(), true);
	CHECK(original.empty());

	auto firstInstructionEquality = destinationCache.at(0).begin()[0].command() == data.storage->at(0).begin()[0].command() && destinationCache.at(0).begin()[0].dest() == data.storage->at(0).begin()[0].dest() && destinationCache.at(0).begin()[0].left() == data.storage->at(0).begin()[0].left();
	CHECK(firstInstructionEquality);
}

/*
TEST_CASE("[BytecodeInterpreter] var declaration from var") {
	auto [script, data] = Interpret("var toto = 4; var titi = toto;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths linear") {
	auto [script, data] = Interpret("var t = 3 + 4 - 1;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 6);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 left subpart") {
	auto [script, data] = Interpret("var t = (3 + 4) * 2;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 right subpart") {
	auto [script, data] = Interpret("var t = 2 * (3 + 4);");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths subparts") {
	auto [script, data] = Interpret("var t = (3 + 4) * (1 + 2);");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 21);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths with var") {
	auto [script, data] = Interpret("var toto = 4; var t = (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 44);
}

TEST_CASE("[BytecodeInterpreter] var expression declaration") {
  auto [script, data] = Interpret("var result = 7 + 3;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] Introducing block sub-variable") {
	auto [script, data] = Interpret("var toto = 4; { var toto = 5; toto + 1; } var t = toto + 1;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), *data.storage);
	auto res = interpreted->variable(0);
	CHECK(res.nodeval<long>() == 5);
}

*/