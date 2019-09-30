#include <iostream>
#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterDataTestContainer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/Value/BytecodeScript.h"

static const auto reservedKeywords = ska::ReservedKeywordsPool{};
static auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
static std::vector<ska::Token> tokens;
static auto readerI = std::unique_ptr<ska::ScriptAST>{};
static auto scriptCacheI = ska::ScriptCacheAST{};
static auto typeCrosserI = ska::TypeCrosser{};

static void ASTFromInputBytecodeInterpreterNoParse(const std::string& input, BytecodeInterpreterDataTestContainer& data) {
  tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
  tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::ScriptAST>(scriptCacheI, "main", tokens);

  data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords);
	data.interpreter = std::make_unique<ska::bytecode::Interpreter>(*data.generator, reservedKeywords);
}

static std::pair<ska::bytecode::ScriptGenerationService, BytecodeInterpreterDataTestContainer> Interpret(const std::string& input) {
	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationService, BytecodeInterpreterDataTestContainer>(ska::bytecode::ScriptGenerationService{0, *readerI }, std::move(data));
}

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};

TEST_CASE("[BytecodeInterpreter] literal alone") {
	auto [script, data] = Interpret("4;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen);
}

TEST_CASE("[BytecodeInterpreter] var declaration") {
	auto [script, data] = Interpret("var toto = 4;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] var declaration from var") {
	auto [script, data] = Interpret("var toto = 4; var titi = toto;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths linear") {
	auto [script, data] = Interpret("var t = 3 + 4 - 1;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 6);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 left subpart") {
	auto [script, data] = Interpret("var t = (3 + 4) * 2;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 right subpart") {
	auto [script, data] = Interpret("var t = 2 * (3 + 4);");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths subparts") {
	auto [script, data] = Interpret("var t = (3 + 4) * (1 + 2);");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 21);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths with var") {
	auto [script, data] = Interpret("var toto = 4; var t = (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 44);
}

TEST_CASE("[BytecodeInterpreter] var expression declaration") {
  auto [script, data] = Interpret("var result = 7 + 3;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] Introducing block sub-variable") {
	auto [script, data] = Interpret("var toto = 4; { var toto = 5; toto + 1; } var t = toto + 1;");
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	CHECK(res.nodeval<long>() == 5);
}

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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
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
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 789);
}

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) and use") {
	constexpr auto progStr = 
		"var Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var player = Character184.build(\"Player\");"
		"var enemy = Character184.default;"
		"var t = enemy.age;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Outside script from file (import) - edit - and use") {
	constexpr auto progStr = 
		"var Character260 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var enemy = Character260.default;"
		"enemy.age = 99;"
		"var t = enemy.age;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 99);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script : ensure we do not try to recompile neither rerun it") {
	constexpr auto progStr =
		"var Character270 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character272 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character270.default.age;"
		"var t = Character272.default.age;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"var Character284 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character285 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character284.default.age = 123;"
		"var t = Character285.default.age;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto res = data.interpreter->interpret(gen.script("main").first,gen)->variable(0);
	auto cellValue = res.nodeval<long>();
	CHECK(cellValue == 123);
}