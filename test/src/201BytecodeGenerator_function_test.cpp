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
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/Value/BytecodeScript.h"

static const auto reservedKeywords = ska::ReservedKeywordsPool{};
static auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
static std::vector<ska::Token> tokens;
static auto readerI = std::unique_ptr<ska::Script>{};
static auto scriptCacheI = ska::ScriptCache{};
static auto typeCrosserI = ska::TypeCrosser{};

static void ASTFromInputBytecodeGeneratorNoParse(const std::string& input, BytecodeGeneratorDataTestContainer& data) {
  tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
  tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::Script>(scriptCacheI, "main", tokens);

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
	CHECK(result.pack().size() == expected.size());
	for(const auto& r : result.pack()) {
		const auto equality =
			index < expected.size() &&
			r.command() == expected[index].command &&
			r.dest().content == expected[index].dest &&
			r.left().content == expected[index].left &&
			r.right().content == expected[index].right;
		CHECK(equality);
		index++;
	}
}

TEST_CASE("[BytecodeGenerator] Empty function only void") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function() { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "2"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::END }
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 1 parameter") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "3"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::END }
	});
}

TEST_CASE("[BytecodeGenerator] Empty function with 4 parameters (> 3)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(t: int, t1: string, t2: int, t3: int) { };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "4"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::POP, "V0", "V1", "V2"},
		{ska::bytecode::Command::POP, "V3" },
		{ska::bytecode::Command::END }
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "2"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::END, "0" }
	});
}

TEST_CASE("[BytecodeGenerator] Basic function with 1 parameter 1 return type") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { return 0; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "3"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::END, "0" }
	});
}

TEST_CASE("[BytecodeGenerator] Function with 1 parameter and some computing inside") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = function(test: int): int { var result = test + 3; return result; };");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP, "5"},
		{ska::bytecode::Command::LABEL, "toto"},
		{ska::bytecode::Command::POP, "V0"},
		{ska::bytecode::Command::ADD_I, "R0", "V0", "3"},
		{ska::bytecode::Command::MOV, "V1", "R0"},
		{ska::bytecode::Command::END, "V1" }
	});
}
