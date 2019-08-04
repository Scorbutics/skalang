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
