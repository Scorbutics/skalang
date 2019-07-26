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
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords, typeCrosserI);
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
	for(const auto& r : result.pack()) {
		CHECK(index < expected.size());
		CHECK(r.command() == expected[index].command);
		CHECK(r.dest().content == expected[index].dest);
		CHECK(r.left().content == expected[index].left);
		CHECK(r.right().content == expected[index].right);
		index++;
	}
}

TEST_CASE("[BytecodeGenerator] literal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "R0", "4"}
	});
}

TEST_CASE("[BytecodeGenerator] var declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4;");
	auto res = data.generator->generate(astPtr);
	
	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "toto", "4"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths linear") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 + 4 - 1;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB, "R0", "4", "1"},
		{ska::bytecode::Command::ADD, "R1", "3", "R0"},
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 left subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * 2;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD, "R0", "3", "4"},
		{ska::bytecode::Command::MUL, "R1", "R0", "2"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 right subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("2 * (3 + 4);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD, "R0", "3", "4"},
		{ska::bytecode::Command::MUL, "R1", "2", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths subparts") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * (1 + 2);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD, "R0", "3", "4"},
		{ska::bytecode::Command::ADD, "R1", "1", "2"},
		{ska::bytecode::Command::MUL, "R2", "R0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths with var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4; (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "toto", "4"},
		{ska::bytecode::Command::MUL, "R0", "toto", "5"},
		{ska::bytecode::Command::DIV, "R1", "1", "4"},
		{ska::bytecode::Command::SUB, "R2", "4", "R1"},
		{ska::bytecode::Command::ADD, "R3", "3", "R2"},
		{ska::bytecode::Command::MUL, "R4", "2", "R3"},
		{ska::bytecode::Command::ADD, "R5", "1", "9"},
		{ska::bytecode::Command::ADD, "R6", "R4", "R5"},	
		{ska::bytecode::Command::ADD, "R7", "R0", "R6"}
	});
}
