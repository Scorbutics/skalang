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
			r.dest().content == expected[index].dest &&
			r.left().content == expected[index].left &&
			r.right().content == expected[index].right;
		CHECK(equality);
		index++;
	}
}

TEST_CASE("[BytecodeGenerator] type conversion + int => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7 + \"3\";");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_STR, "R0", "7"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

/*
//TODO : conversion unsupported atm

TEST_CASE("[BytecodeGenerator] type conversion + float => string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7.0 + \"3\";");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_D_STR, "R1", "7.0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R1", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}
*/

TEST_CASE("[BytecodeGenerator] type conversion + int => float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7.0 + 3;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::CONV_I_D, "R0", "3"},
		{ska::bytecode::Command::ADD_D, "R0", "7.0", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (back)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = [7, 12, 25] + 3;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0"},
		{ska::bytecode::Command::PUSH_B_ARR, "R1", "R0", "3"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] type conversion + int => array (front)") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3 + [7, 12, 25];");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R0"},
		{ska::bytecode::Command::PUSH_F_ARR, "R1", "3", "R0"},
		{ska::bytecode::Command::MOV, "V0", "R1"}
	});
}
