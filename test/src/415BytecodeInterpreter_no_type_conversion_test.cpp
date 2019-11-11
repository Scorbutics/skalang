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
#include "Generator/Value/BytecodeScriptGenerationService.h"

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

static std::pair<ska::bytecode::ScriptGenerationService, BytecodeInterpreterDataTestContainer> Interpret(const std::string& input) {
	auto data = BytecodeInterpreterDataTestContainer{};
	ASTFromInputBytecodeInterpreterNoParse(input, data);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationService, BytecodeInterpreterDataTestContainer>(ska::bytecode::ScriptGenerationService{0, *readerI }, std::move(data));
}

TEST_CASE("[BytecodeInterpreter] no type conversion string + string") {
	static constexpr auto progStr = "var result = \"3\" + \"7\";";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::StringShared>();
	CHECK(*firstCellValue == "37");
}

TEST_CASE("[BytecodeInterpreter] no type conversion float + float") {
	static constexpr auto progStr = "var result = 3.0 + 7.4;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	CHECK(firstCellValue == 10.4);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array") {
	static constexpr auto progStr = "var result = [3];";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
	CHECK((*firstCellValue)[0].nodeval<long>() == 3L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array + array") {
	static constexpr auto progStr = "var result = [3] + [7, 12, 25];";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 4);
  CHECK((*firstCellValue)[0].nodeval<long>() == 3L);
  CHECK((*firstCellValue)[1].nodeval<long>() == 7L);
  CHECK((*firstCellValue)[2].nodeval<long>() == 12L);
  CHECK((*firstCellValue)[3].nodeval<long>() == 25L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int - int") {
	static constexpr auto progStr = "var result = 3 - 4;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == -1);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float - float") {
	static constexpr auto progStr = "var result = 3.1 - 4.2;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
	CHECK(firstCellValue == -1.1);
}

TEST_CASE("[BytecodeInterpreter] no type conversion array - array") {
	static constexpr auto progStr = "var result = [11, 4, 9] - [1, 2];";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<ska::NodeValueArray>();
  CHECK(firstCellValue->size() == 1);
  CHECK((*firstCellValue)[0].nodeval<long>() == 11L);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int * int") {
	static constexpr auto progStr = "var result = 3 * 4;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 12);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float * float") {
	static constexpr auto progStr = "var result = 3.0 * 4.4;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
  const auto check = firstCellValue + 0.01 >= 13.2 && firstCellValue - 0.01 <= 13.2;
	CHECK(check);
}

TEST_CASE("[BytecodeInterpreter] no type conversion int / int") {
	static constexpr auto progStr = "var result = 3 / 4;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<long>();
	CHECK(firstCellValue == 0);
}

TEST_CASE("[BytecodeInterpreter] no type conversion float / float") {
	static constexpr auto progStr = "var result = 3.0 / 4.4;";
  auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto firstCellValue = res.nodeval<double>();
  const auto check = firstCellValue + 0.000001 >= 0.68181818181 && firstCellValue - 0.000001 <= 0.68181818181;
	CHECK(check);
}
