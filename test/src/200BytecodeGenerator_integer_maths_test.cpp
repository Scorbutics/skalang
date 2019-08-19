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

TEST_CASE("[BytecodeGenerator] literal alone") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("4;");
	auto res = data.generator->generate(astPtr);
	BytecodeCompare(res, { });
}

TEST_CASE("[BytecodeGenerator] var declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"}
	});
}

TEST_CASE("[BytecodeGenerator] var declaration from var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4; var titi = toto;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"},
		{ska::bytecode::Command::MOV, "V1", "V0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths linear") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("3 + 4 - 1;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB_I, "R0", "4", "1"},
		{ska::bytecode::Command::ADD_I, "R1", "3", "R0"},
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 left subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * 2;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::MUL_I, "R1", "R0", "2"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths 1 right subpart") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("2 * (3 + 4);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::MUL_I, "R1", "2", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths subparts") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("(3 + 4) * (1 + 2);");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "3", "4"},
		{ska::bytecode::Command::ADD_I, "R1", "1", "2"},
		{ska::bytecode::Command::MUL_I, "R2", "R0", "R1"}
	});
}

TEST_CASE("[BytecodeGenerator] Basic Maths with var") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4; (toto * 5) + 2 * (3 + 4 - 1 / 4) + 1 + 9;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"},
		{ska::bytecode::Command::MUL_I, "R0", "V0", "5"},
		{ska::bytecode::Command::DIV_I, "R1", "1", "4"},
		{ska::bytecode::Command::SUB_I, "R2", "4", "R1"},
		{ska::bytecode::Command::ADD_I, "R3", "3", "R2"},
		{ska::bytecode::Command::MUL_I, "R4", "2", "R3"},
		{ska::bytecode::Command::ADD_I, "R5", "1", "9"},
		{ska::bytecode::Command::ADD_I, "R6", "R4", "R5"},
		{ska::bytecode::Command::ADD_I, "R7", "R0", "R6"}
	});
}

TEST_CASE("[BytecodeGenerator] var expression declaration") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 7 + 3;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_I, "R0", "7", "3"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] Introducing block sub-variable") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var toto = 4; { var toto = 5; toto + 1; } toto + 1;");
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::MOV, "V0", "4"},
		{ska::bytecode::Command::MOV, "V1", "5"},
		{ska::bytecode::Command::ADD_I, "R0", "V1", "1"},
		{ska::bytecode::Command::ADD_I, "R1", "V0", "1"}
	});
}
