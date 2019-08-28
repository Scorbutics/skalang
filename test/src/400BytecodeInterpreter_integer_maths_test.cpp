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
	data.interpreter = std::make_unique<ska::bytecode::Interpreter>(reservedKeywords);
}

static std::pair<ska::bytecode::Script, BytecodeInterpreterDataTestContainer> Interpret(const std::string& input) {
	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::Script, BytecodeInterpreterDataTestContainer>(ska::bytecode::Script{ *readerI }, std::move(data));
}

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};

TEST_CASE("[BytecodeInterpreter] literal alone") {
	auto [script, data] = Interpret("4;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
}

TEST_CASE("[BytecodeInterpreter] var declaration") {
	auto [script, data] = Interpret("var toto = 4;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] var declaration from var") {
	auto [script, data] = Interpret("var toto = 4; var titi = toto;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 4);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths linear") {
	auto [script, data] = Interpret("3 + 4 - 1;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 6);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 left subpart") {
	auto [script, data] = Interpret("(3 + 4) * 2;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths 1 right subpart") {
	auto [script, data] = Interpret("2 * (3 + 4);");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 14);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths subparts") {
	auto [script, data] = Interpret("(3 + 4) * (1 + 2);");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 21);
}

TEST_CASE("[BytecodeInterpreter] Basic Maths with var") {
	auto [script, data] = Interpret("var toto = 4; (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 44);
}

TEST_CASE("[BytecodeInterpreter] var expression declaration") {
  auto [script, data] = Interpret("var result = 7 + 3;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 10);
}

TEST_CASE("[BytecodeInterpreter] Introducing block sub-variable") {
	auto [script, data] = Interpret("var toto = 4; { var toto = 5; toto + 1; } toto + 1;");
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 5);
}

TEST_CASE("[BytecodeInterpreter] Custom object creation") {
	constexpr auto progStr =
		"var toto = function() : var {"
			"var priv_test = 1;"
			"return {"
				"test : priv_test,"
				"say : function(more : string) : string {"
					"var s = \"lol\" + priv_test + more;"
					"return s;"
				"}"
			"};"
		"};"
		"var test = toto();"
		"test.test;";

	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(script);
	auto res = data.interpreter->interpret(gen);
	CHECK(res.nodeval<long>() == 5);
}
