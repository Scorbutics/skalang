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
	data.generator = std::make_unique<ska::BytecodeGenerator>(reservedKeywords, typeCrosserI);
}

static std::pair<ska::BytecodeScript, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = BytecodeGeneratorDataTestContainer{};
	ASTFromInputBytecodeGeneratorNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::BytecodeScript, BytecodeGeneratorDataTestContainer>(ska::BytecodeScript{ *readerI }, std::move(data));
}

struct BytecodePart {
	ska::BytecodeCommand command;
	std::string value;
};

static void BytecodeCompare(const ska::BytecodeCellGroup& result, std::vector<BytecodePart> expected) {
	auto index = std::size_t {0};
	for(const auto& r : result) {
		CHECK(index < expected.size());
		CHECK(r.command() == expected[index].command);
		CHECK(r.value().name() == expected[index].value);
		index++;
	}
}

TEST_CASE("[BytecodeGenerator] literal") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4;");
	auto res = data.generator->generate(astPtr);
}

TEST_CASE("[BytecodeGenerator] var declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4;");
	auto res = data.generator->generate(astPtr);
}

TEST_CASE("[BytecodeGenerator] Basic Maths linear") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 + 4 - 1;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::BytecodeCommand::OUT, "4"},
		{ska::BytecodeCommand::SUB, "1"},
		{ska::BytecodeCommand::IN, " 0"},
		{ska::BytecodeCommand::OUT, "3"},
		{ska::BytecodeCommand::ADD, " 0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 left subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * 2;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::BytecodeCommand::OUT, "2"},
		{ska::BytecodeCommand::IN, " 0"},
		{ska::BytecodeCommand::OUT, "3"},
		{ska::BytecodeCommand::ADD, "4"},
		{ska::BytecodeCommand::MUL, " 0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 right subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("2 * (3 + 4);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::BytecodeCommand::OUT, "3"},
		{ska::BytecodeCommand::ADD, "4"},
		{ska::BytecodeCommand::IN, " 0"},
		{ska::BytecodeCommand::OUT, "2"},
		{ska::BytecodeCommand::MUL, " 0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths subparts") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * (1 + 2);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::BytecodeCommand::OUT, "1"},
		{ska::BytecodeCommand::ADD, "2"},
		{ska::BytecodeCommand::IN, " 1"},
		{ska::BytecodeCommand::OUT, "3"},
		{ska::BytecodeCommand::ADD, "4"},
		{ska::BytecodeCommand::IN, " 0"},
		{ska::BytecodeCommand::MUL, " 1"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths with var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;");
	auto res = data.generator->generate(astPtr);

	// 	Expected : 
	
	// 	out 4
	// 	in toto
	// 	out toto
	// 	out toto
	// 	mul 5
	// 	in v0
	// 	out v0
	// 	out 3
	// 	add 4
	// 	in v1
	// 	sub 1
	// 	in v2
	// 	out 2
	// 	mul v2
	// 	in v3
	// 	add 1
	// 	in v4
	// 	add 9
}


