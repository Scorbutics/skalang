#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeGeneratorDataTestContainer.h"
#include "BytecodeCompare.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"

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

static std::pair<ska::bytecode::ScriptGenerationService, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = BytecodeGeneratorDataTestContainer{};
	ASTFromInputBytecodeGeneratorNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationService, BytecodeGeneratorDataTestContainer>(ska::bytecode::ScriptGenerationService{0, *readerI }, std::move(data));
}

TEST_CASE("[BytecodeGenerator] no type conversion string + string") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = \"3\" + \"7\";");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_STR, "R0", "3", "7"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float + float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3.0 + 7.4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::ADD_D, "R0", "3.000000", "7.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = [3];");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "3"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "1"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array + array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = [3] + [7, 12, 25];");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "3"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "1"},
		{ska::bytecode::Command::PUSH, "7", "12", "25"},
		{ska::bytecode::Command::POP_IN_ARR, "R1", "3"},
		{ska::bytecode::Command::PUSH_ARR_ARR, "R2", "R0", "R1"},
		{ska::bytecode::Command::MOV, "V0", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int - int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3 - 4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float - float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3.1 - 4.2;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::SUB_D, "R0", "3.100000", "4.200000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion array - array") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = [11, 4, 9] - [1, 2];");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::PUSH, "11", "4", "9"},
		{ska::bytecode::Command::POP_IN_ARR, "R0", "3"},
		{ska::bytecode::Command::PUSH, "1", "2"},
		{ska::bytecode::Command::POP_IN_ARR, "R1", "2"},
		{ska::bytecode::Command::SUB_ARR, "R2", "R0", "R1"},
		{ska::bytecode::Command::MOV, "V0", "R2"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int * int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3 * 4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MUL_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float * float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3.0 * 4.4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::MUL_D, "R0", "3.000000", "4.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion int / int") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3 / 4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::DIV_I, "R0", "3", "4"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}

TEST_CASE("[BytecodeGenerator] no type conversion float / float") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var result = 3.0 / 4.4;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ska::bytecode::Command::DIV_D, "R0", "3.000000", "4.400000"},
		{ska::bytecode::Command::MOV, "V0", "R0"}
	});
}
