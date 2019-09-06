#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorDataTestContainer.h"
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

static void ASTFromInputBytecodeGeneratorNoParse(const std::string& input, BytecodeGeneratorDataTestContainer& data) {
  tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
  tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::ScriptAST>(scriptCacheI, "main", tokens);

  data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords);
}

static std::pair<ska::bytecode::Script, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = BytecodeGeneratorDataTestContainer{};
	ASTFromInputBytecodeGeneratorNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::Script, BytecodeGeneratorDataTestContainer>(ska::bytecode::Script{ *readerI }, std::move(data));
}

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};

static void BytecodeCompare(const ska::bytecode::GenerationOutput& result, std::vector<BytecodePart> expected) {
	auto index = std::size_t {0};
	CHECK(result.size() == expected.size());
	for(const auto& r : result) {
		const auto equality =
			index < expected.size() &&
			r.command() == expected[index].command &&
			r.dest().toString() == expected[index].dest &&
			r.left().toString() == expected[index].left &&
			r.right().toString() == expected[index].right;
		CHECK(equality);
		index++;
	}
}

TEST_CASE("[BytecodeGenerator] Empty function only void") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function() { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V0", "-2" }
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 1 parameter") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V1", "-3"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int, t1: string, t2: int, t3: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "3"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::RET},
		{ska::bytecode::Command::END, "V4", "-4"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "1"},
		{ska::bytecode::Command::RET, "0"},
		{ska::bytecode::Command::END, "V0", "-2" }
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "2"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::RET, "0"},
		{ska::bytecode::Command::END, "V1", "-3" }
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { var result = test + 3; return result; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "4"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::ADD_I, "R0", "V0", "3"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::RET, "V1"},
		{ska::bytecode::Command::END, "V2", "-5" }
	});
}


TEST_CASE("[BytecodeGenerator] Custom object creation") {
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
		"var test = toto();";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "1"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "1"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"}
	});
}

TEST_CASE("[BytecodeGenerator] Custom object creation2") {
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
		"test.say(\"titi\");";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto res = data.generator->generate(script);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "1"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"},
		{ska::bytecode::Command::ARR_ACCESS, "R4", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "5"},
		{ska::bytecode::Command::POP, "R5"}
	});
}

// We have to check that calling a function several times stills refer to the same
// JUMP_ABS instruction
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
		"test.say(\"titi4\");";

	auto [script, data] = ASTFromInputBytecodeGenerator(progStr);
	auto res = data.generator->generate(script);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "123"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "1"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"},
		{ska::bytecode::Command::ARR_ACCESS, "R4", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi"},
		{ska::bytecode::Command::JUMP_ABS, "5"},
		{ska::bytecode::Command::POP, "R5"},
		{ska::bytecode::Command::ARR_ACCESS, "R6", "V6", "1"},
		{ska::bytecode::Command::PUSH, "titi4"},
		{ska::bytecode::Command::JUMP_ABS, "5"},
		{ska::bytecode::Command::POP, "R7"}
	});
}
