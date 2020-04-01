#include <iostream>
#include <iomanip>
#include <doctest.h>
#include <tuple>

#include "BytecodeSerializerTest.h"
#include "Serializer/Symbol/BytecodeSymbolTableSerializer.h"
#include "Serializer/Symbol/BytecodeSymbolTableDeserializer.h"
#include "Base/Serialization/SerializerOutput.h"

#include "NodeValue/ScriptAST.h"

static std::unordered_map<std::string, std::stringstream> SerializingStreams = {};

TEST_CASE("[BytecodeSymbolTableSerializer] 2 scopes levels test") {
	
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;

	{
		auto [script, data] = Serialize(
			"toto = 1\n"
			"tata = 123\n"
			"do\n"
			"tutu = 3\n"
			"titi = 12\n"
			"end\n"
			"toutou = 5\n"
		);
		auto& gen = data.generator->generate(*data.storage, std::move(script));
		auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

	}

	{
		auto [script, data] = Serialize("");

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);
	}
}

TEST_CASE("[BytecodeSymbolTableSerializer] 3 scopes levels test") {
	auto [script, data] = Serialize(
		"toto = 1\n"
		"tata = 123\n"
		"do\n"
		"tutu = 3\n"
		"do\n"
		"tiutiu = 321\n"
		"end\n"
		"titi = 12\n"
		"end\n"
		"toutou = 5\n"
	);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	auto input = ska::SerializerOutput{ {ss, natives} };
	symbolTableSerializer.writeFull(std::move(input), gen.id());
}

TEST_CASE("[BytecodeSymbolTableSerializer] function var factory test") {
	auto [script, data] = Serialize(
		"toto = function() : var do\n"
		"test = 3\n"
		"return { \n"
		"test = test\n"
		"tata = \"44\"\n"
		"} end\n"
		"tata = 123\n"
		"titi = toto()\n"
		"titi.test = 5\n"
	);
	auto& gen = data.generator->generate(*data.storage, std::move(script));
	auto symbolTableSerializer = ska::bytecode::SymbolTableSerializer{ *data.storage };
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	auto input = ska::SerializerOutput{ {ss, natives} };
	symbolTableSerializer.writeFull(std::move(input), gen.id());
}


