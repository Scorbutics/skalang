#include <iostream>
#include <iomanip>
#include <doctest.h>
#include <tuple>

#include "BytecodeSerializerTest.h"
#include "Serializer/BytecodeSymbolTableSerializer.h"

static std::unordered_map<std::string, std::stringstream> SerializingStreams = {};

TEST_CASE("[BytecodeTreeSymbolTableSerializer] 2 levels test") {
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
	std::stringstream ss;
	std::unordered_map<std::string, std::size_t> natives;
	symbolTableSerializer.writeFull(gen.id(), natives, ss);
}

TEST_CASE("[BytecodeTreeSymbolTableSerializer] 3 levels test") {
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
	std::unordered_map<std::string, std::size_t> natives;
	symbolTableSerializer.writeFull(gen.id(), natives, ss);
}

