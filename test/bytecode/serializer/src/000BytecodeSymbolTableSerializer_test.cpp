#include <iostream>
#include <iomanip>
#include <doctest.h>
#include <tuple>

#include "BytecodeSerializerTest.h"
#include "Serializer/Symbol/BytecodeSymbolTableSerializer.h"
#include "Serializer/Symbol/BytecodeSymbolTableDeserializer.h"
#include "Base/Serialization/SerializerOutput.h"

#include "NodeValue/ScriptAST.h"

TEST_CASE("[BytecodeSymbolTableSerializer] 1 scope level test + type") {

	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;

	{
		auto [script, data] = Serialize(
			"toto = 1\n"
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

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.scopes() == 0);
		CHECK(deserializedSymbolTable.size() == 1);
		const auto* totoSymbol = deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto"));
		CHECK(totoSymbol != nullptr);
		CHECK(totoSymbol->type() == ska::ExpressionType::INT);
		CHECK(totoSymbol->type().size() == 0);
	}
}

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

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.scopes() == 1);
		CHECK(deserializedSymbolTable.size() == 3);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tata")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toutou")) != nullptr);
		
		const auto* childScope = deserializedSymbolTable.root().child(0);
		CHECK(childScope != nullptr);
		CHECK(childScope->scopes() == 0);
		CHECK(childScope->size() == 2);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tutu"), ska::SymbolTableNested::firstChild()) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("titi"), ska::SymbolTableNested::firstChild()) != nullptr);
	}
}

TEST_CASE("[BytecodeSymbolTableSerializer] 3 scopes levels test") {
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	{
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
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());

	}

	{
		auto [script, data] = Serialize("");

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.scopes() == 1);
		CHECK(deserializedSymbolTable.size() == 3);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toto")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tata")) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("toutou")) != nullptr);

		const auto* childScope = deserializedSymbolTable.root().child(0);
		CHECK(childScope != nullptr);
		CHECK(childScope->scopes() == 1);
		CHECK(childScope->size() == 2);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tutu"), ska::SymbolTableNested::firstChild()) != nullptr);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("titi"), ska::SymbolTableNested::firstChild()) != nullptr);

		const auto* secondLevelChildScope = childScope->child(0);
		CHECK(secondLevelChildScope != nullptr);
		CHECK(secondLevelChildScope->scopes() == 0);
		CHECK(secondLevelChildScope->size() == 1);
		CHECK(deserializedSymbolTable.lookup(ska::SymbolTableLookup::direct("tiutiu"), ska::SymbolTableNested::firstChild(2)) != nullptr);
	}
}

TEST_CASE("[BytecodeSymbolTableSerializer] function var factory test") {
	std::stringstream ss;
	ska::order_indexed_string_map<std::string> natives;
	{
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
		auto input = ska::SerializerOutput{ {ss, natives} };
		symbolTableSerializer.writeFull(std::move(input), gen.id());
	}

	{
		auto [script, data] = Serialize("");

		auto symbolTableDeserializer = ska::bytecode::SymbolTableDeserializer{ *data.storage };
		auto output = ska::SerializerOutput{ {ss, natives} };
		symbolTableDeserializer.readFull(output);

		const auto& deserializedSymbolTable = script.program().symbols();
		CHECK(deserializedSymbolTable.scopes() == 1);
		CHECK(deserializedSymbolTable.size() == 3);
	}
}


