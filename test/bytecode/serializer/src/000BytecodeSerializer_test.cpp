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
	auto [script, data] = Serialize("4\n");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream());
	CHECK(original.empty());

	// A way to test instructions emptyness
	CHECK(destinationCache.at(0).begin() == destinationCache.at(0).end());
}

TEST_CASE("[BytecodeSerializer] function + field access + floating point value + integer value + string value") {
	auto [script, data] = Serialize("toto = function(): var do return { bark = \"bibi\" }\n end\n toto().bark\n \"test\"\n 3.4\n 3\n");
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream());
	CHECK(original.empty());

	CHECK(destinationCache.at(0) == data.storage->at(0));
}


TEST_CASE("[BytecodeSerializer] concrete external script use") {
	constexpr auto progStr =
		"Character66 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"enemy = Character66.default\n"
		"enemy.age = 99\n"
		"t = enemy.age\n";
	auto [script, data] = Serialize(progStr);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream());
	CHECK(original.empty());

	CHECK(destinationCache.size() == 2);
	const auto equality = destinationCache.at(0) == data.storage->at(0) && destinationCache.at(1) == data.storage->at(1);
	CHECK(equality);
}

TEST_CASE("[BytecodeSerializer] binded external script use") {
	constexpr auto progStr =
		"Logger = import \"bind:std.native.io.log\"\n"
		"Logger.print(\"test63\")\n";

	
	auto data = BytecodeSerializerDataTestContainer {};
	ASTFromInputBytecodeSerializerNoParse(progStr, data);
	
	auto interpreter = ska::bytecode::Interpreter{ *data.parser, *data.generator, data.reservedKeywords };
	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>{ data.storage->astCache, *data.typeBuilder, *data.typeChecker, reservedKeywords, *data.parser, *data.storage, interpreter };
	auto logModule = ska::lang::IOLogModule<ska::bytecode::Interpreter>(moduleConfiguration);
	
	readerI->parse(*data.parser);
	auto script = ska::bytecode::ScriptGenerationHelper{ *data.storage, *readerI };

	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto res = data.serializer->serialize(*data.storage, SerializeInStream());
	CHECK(res);
	auto destinationCache = ska::bytecode::ScriptCache{};
	auto original = data.serializer->deserialize(destinationCache, "main", DeserializeInStream());
	CHECK(original.empty());

	CHECK(destinationCache.size() == 2);
	const auto equality = destinationCache.at(1) == data.storage->at(1) && destinationCache.at(0).size() == 0;
	CHECK(equality);
}

TEST_CASE("[BytecodeSerializer] external script use, other stack, triggers rebuild") {
	
	std::unordered_map<std::string, std::stringstream> serializingStreamsOut = {};
	auto stringSerializer = [&serializingStreamsOut](const std::string& scriptName) -> std::ostream& {
		std::cout << "local serialization " << scriptName << std::endl;
		return serializingStreamsOut[scriptName];
	};

	auto stringDeserializer = [&serializingStreamsOut](const std::string& scriptName) -> std::istream& {
		std::cout << "local deserialization " << scriptName << std::endl;
		return serializingStreamsOut[scriptName];
	};


	constexpr auto progStr =
		"Character66 = import \"" SKALANG_TEST_DIR "/src/resources/character\"\n"
		"enemy = Character66.default\n"
		"enemy.age = 99\n"
		"t = enemy.age\n";

	{
		auto data = BytecodeSerializerDataTestContainer{};
		ASTFromInputBytecodeSerializerNoParse(progStr, data);

		auto interpreter = ska::bytecode::Interpreter{ *data.parser, *data.generator, data.reservedKeywords };
	
		auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>{ data.storage->astCache, *data.typeBuilder, *data.typeChecker, reservedKeywords, *data.parser, *data.storage, interpreter };
		auto logModule = ska::lang::IOLogModule<ska::bytecode::Interpreter>(moduleConfiguration);

		readerI->parse(*data.parser);
		auto script = ska::bytecode::ScriptGenerationHelper{ *data.storage, *readerI };

		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto res = data.serializer->serialize(*data.storage, stringSerializer);
		CHECK(res);
		readerI = nullptr;
		tokenizer = {};
		tokens = {};
	}
	{
		auto data = BytecodeSerializerDataTestContainer{};
		ASTFromInputBytecodeSerializerNoParse(progStr, data);

		auto interpreter = ska::bytecode::Interpreter{ *data.parser, *data.generator, data.reservedKeywords };

		auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>{ data.storage->astCache, *data.typeBuilder, *data.typeChecker, reservedKeywords, *data.parser, *data.storage, interpreter };
		auto original = data.serializer->deserialize(*data.storage, "main", stringDeserializer, { "main" });
		CHECK(original.empty());

		auto logModule = ska::lang::IOLogModule<ska::bytecode::Interpreter>(moduleConfiguration);

		readerI->parse(*data.parser);
		auto script = ska::bytecode::ScriptGenerationHelper{ *data.storage, *readerI };
		auto& gen = data.generator->generate(*data.storage, std::move(script));


		CHECK(data.storage->size() == 2);
		const auto equality = data.storage->at(1) == data.storage->at(1) && data.storage->at(0).size() == 0;
		CHECK(equality);
	}
}
