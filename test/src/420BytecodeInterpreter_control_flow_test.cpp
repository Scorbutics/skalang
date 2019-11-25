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

TEST_CASE("[BytecodeInterpreter] if : if body") {
	static constexpr auto progStr =
	"var t = false;"
	"if ([18] == [18]) {"
	"t = true;"
	"}";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<bool>();
  CHECK(cellValue == true);
}

TEST_CASE("[BytecodeInterpreter] if : else body") {
	static constexpr auto progStr =
	"var t = 0;"
	"if ([18] == [1124]) {"
	"t = 1;"
	"} else {"
	"t = 2;"
	"}";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 2);
}

TEST_CASE("[BytecodeInterpreter] for without body") {
	static constexpr auto progStr = "for(var i = 0; i < 10; i = i + 1);";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 10);
}

TEST_CASE("[BytecodeInterpreter] for with body") {
	static constexpr auto progStr = "var toto = 123; for(var i = 0; i < 10; i = i + 1) { toto = toto + i * 2; } var test = toto;";
	auto [script, data] = Interpret(progStr);
	auto gen = data.generator->generate(data.storage, std::move(script));
	auto interpreted = data.interpreter->interpret(gen.id(), data.storage);
	auto res = interpreted->variable(0);
	auto cellValue = res.nodeval<long>();
  CHECK(cellValue == 213);
}
