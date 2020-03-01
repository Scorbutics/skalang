#include <iostream>
#include <iomanip>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"

#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "BytecodeSerializerTest.h"
#include "std/module/io/log.h"

static std::unordered_map<std::string, std::stringstream> SerializingStreams = {};

ska::bytecode::SerializationStrategy SerializeInStream() {
	return [](const std::string& scriptName) -> std::ostream& {
		std::cout << "serializing " << scriptName << std::endl;
		return SerializingStreams[scriptName];
	};
}

ska::bytecode::DeserializationStrategy DeserializeInStream() {
	return [](const std::string& scriptName) -> std::istream& {
		std::cout << "deserializing " << scriptName << std::endl;
		return SerializingStreams[scriptName];
	};
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

TEST_CASE("[BytecodeSerializer] function + field access + floating point value + integer value + string value") {
	auto [script, data] = Serialize("var toto = function(): var { return { bark : \"bibi\" }; }; toto().bark; \"test\"; 3.4; 3;");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream(), true);
	CHECK(original.empty());

	const auto equality = destinationCache.at(0) == data.storage->at(0);
	CHECK(equality);
}


TEST_CASE("[BytecodeSerializer] concrete external script use") {
	constexpr auto progStr =
		"var Character66 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var enemy = Character66.default;"
		"enemy.age = 99;"
		"var t = enemy.age;";
	auto [script, data] = Serialize(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream(), true);
	CHECK(original.empty());

	CHECK(destinationCache.size() == 2);
	const auto equality = destinationCache.at(0) == data.storage->at(0) && destinationCache.at(1) == data.storage->at(1);
	CHECK(equality);
}

TEST_CASE("[BytecodeSerializer] binded external script use") {
	std::cout << std::endl;
	std::cout << std::endl;
	constexpr auto progStr =
		"var Logger = import \"bind:std.native.io.log\";"
		"Logger.print(\"test63\");";

	
	auto data = BytecodeSerializerDataTestContainer {};
	ASTFromInputBytecodeSerializerNoParse(progStr, data);
	
	auto interpreter = ska::bytecode::Interpreter{ *data.parser, *data.generator, data.reservedKeywords };
	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>{ data.storage->astCache, *data.typeBuilder, *data.symbolsTypeUpdater, *data.typeChecker, reservedKeywords, *data.parser, *data.storage, interpreter };
	auto logModule = ska::lang::IOLogModule<ska::bytecode::Interpreter>(moduleConfiguration);
	
	readerI->parse(*data.parser);
	auto script = ska::bytecode::ScriptGenerationHelper{ *data.storage, *readerI };

	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream(), true);
	CHECK(original.empty());

	CHECK(destinationCache.size() == 2);
	const auto equality = destinationCache.at(1) == data.storage->at(1) && destinationCache.at(0).size() == 0;
	CHECK(equality);
}
