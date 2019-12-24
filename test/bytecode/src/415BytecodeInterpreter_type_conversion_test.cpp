#include <doctest.h>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreterDataTestContainer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

static const auto reservedKeywords = ska::ReservedKeywordsPool{};
static auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
static std::vector<ska::Token> tokens;
static auto readerI = std::unique_ptr<ska::ScriptAST>{};
static auto scriptCacheI = ska::ScriptCacheAST{};
static auto typeCrosserI = ska::TypeCrosser{};

static void ASTFromInputBytecodeInterpreterNoParse(const std::string& input, BytecodeInterpreterDataTestContainer& data) {
  tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
  tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::ScriptAST>(scriptCacheI, "main", tokens);

  data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords);
	data.interpreter = std::make_unique<ska::bytecode::Interpreter>(*data.generator, reservedKeywords);
}

static std::pair<ska::bytecode::ScriptGenerationHelper, BytecodeInterpreterDataTestContainer> Interpret(const std::string& input) {
	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationHelper, BytecodeInterpreterDataTestContainer>(ska::bytecode::ScriptGenerationHelper{0, *readerI }, std::move(data));
}

TEST_CASE("[BytecodeInterpreter] type conversion + int => string") {
	static constexpr auto progStr = "var result = 7 + \"3\";";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "73");
}

/*
//TODO : conversion unsupported atm

TEST_CASE("[BytecodeInterpreter] type conversion + float => string") {
	static constexpr auto progStr = "var result = 7.0 + \"3\";";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
  CHECK(*firstCellValue == "7.03");
}
*/

TEST_CASE("[BytecodeInterpreter] type conversion + int => float") {
	static constexpr auto progStr = "var result = 7.0 + 3;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	const auto check = firstCellValue + 0.01 >= 10.0 && firstCellValue - 0.01 <= 10.0;
  CHECK(check);
}

TEST_CASE("[BytecodeInterpreter] type conversion + int => array (back)") {
	static constexpr auto progStr = "var result = [7, 12, 25] + 3;";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 4);
	CHECK((*firstCellValue)[0].nodeval<long>() == 7);
  CHECK((*firstCellValue)[1].nodeval<long>() == 12);
  CHECK((*firstCellValue)[2].nodeval<long>() == 25);
  CHECK((*firstCellValue)[3].nodeval<long>() == 3);
}

TEST_CASE("[BytecodeInterpreter] type conversion + int => array (front)") {
	static constexpr auto progStr = "var result = 3 + [7, 12, 25];";
	auto [script, data] = Interpret(progStr);
	auto& gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
	CHECK(firstCellValue->size() == 4);
	CHECK((*firstCellValue)[0].nodeval<long>() == 3);
  CHECK((*firstCellValue)[1].nodeval<long>() == 7);
  CHECK((*firstCellValue)[2].nodeval<long>() == 12);
  CHECK((*firstCellValue)[3].nodeval<long>() == 25);
}
