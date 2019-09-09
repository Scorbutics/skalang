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

static std::pair<ska::bytecode::ScriptGeneration, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = BytecodeGeneratorDataTestContainer{};
	ASTFromInputBytecodeGeneratorNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGeneration, BytecodeGeneratorDataTestContainer>(ska::bytecode::ScriptGeneration{ *readerI }, std::move(data));
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

using namespace ska::bytecode;

TEST_CASE("[BytecodeGenerator] equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4 == 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] var declaration : conditional") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4 == 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"},
		{Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] conditional strings") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("\"4\" == \"1\";");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::CMP_STR, "R0", "4", "1"},
		{Command::TEST_EQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] conditional arrays") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("[4] == [1];");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::PUSH, "4"},
		{Command::POP_IN_ARR, "R0"},
		{Command::PUSH, "1"},
		{Command::POP_IN_ARR, "R1"},
		{Command::CMP_ARR, "R2", "R0", "R1"},
		{Command::TEST_EQ, "R2", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] different") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 != 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_NEQ, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] greater than") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 > 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_G, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] greater than or equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 >= 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_GE, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] lesser than") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 < 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_L, "R0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] lesser than or equal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 <= 1;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, {
		{Command::SUB_I, "R0", "3", "1"},
		{Command::TEST_LE, "R0", "R0"}
	});
}
