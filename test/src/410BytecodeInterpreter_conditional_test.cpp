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

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};

TEST_CASE("[BytecodeInterpreter] equal int (false)") {
	static constexpr auto progStr = "var t = 4 == 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] equal int (true)") {
	static constexpr auto progStr = "var t = 41 == 41;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] conditional strings") {
	static constexpr auto progStr = "var t = \"4\" == \"1\";";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);

}

TEST_CASE("[BytecodeInterpreter] different") {
	static constexpr auto progStr = "var t = 3 != 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] greater than") {
	static constexpr auto progStr = "var t = 3 > 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] greater than or equal") {
	static constexpr auto progStr = "var t = 3 >= 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] lesser than") {
	static constexpr auto progStr = "var t = 3 < 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] lesser than or equal") {
	static constexpr auto progStr = "var t = 3 <= 1;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}

TEST_CASE("[BytecodeInterpreter] conditional arrays") {
	static constexpr auto progStr = "var t = [4] == [1];";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.script("main").first, gen);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == false);
}
