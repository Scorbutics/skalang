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
		{ska::bytecode::Command::JUMP, "2"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::END, "L0" },
		{ska::bytecode::Command::LABEL_AS_REF, "V0", "L0"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 1 parameter") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "3"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::END, "L0"},
		{ska::bytecode::Command::LABEL_AS_REF, "V1", "L0"}
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int, t1: string, t2: int, t3: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "4"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::END, "L0"},
		{ska::bytecode::Command::LABEL_AS_REF, "V4", "L0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "2"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::END, "L0", "0" },
		{ska::bytecode::Command::LABEL_AS_REF, "V0", "L0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "3"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::END, "L0", "0" },
		{ska::bytecode::Command::LABEL_AS_REF, "V1", "L0"}
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { var result = test + 3; return result; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "5"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::ADD_I, "R0", "V0", "3"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::END, "L0", "V1" },
		{ska::bytecode::Command::LABEL_AS_REF, "V2", "L0"}
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
		{ska::bytecode::Command::JUMP, "16"},
		{ska::bytecode::Command::LABEL, "L0"},
		{ska::bytecode::Command::MOV, "V0", "1"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP, "7"},
		{ska::bytecode::Command::LABEL, "L1"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::END, "L1", "V3"},
		{ska::bytecode::Command::LABEL_AS_REF, "V4", "L1"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"},
		{ska::bytecode::Command::END, "L0", "R2"},
		{ska::bytecode::Command::LABEL_AS_REF, "V5", "L0"},
		{ska::bytecode::Command::CALL, "V5"},
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"}
	});
}
