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
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Service/ScriptNameBuilder.h"

static const auto reservedKeywords = ska::ReservedKeywordsPool{};
static auto tokenizer = std::unique_ptr<ska::Tokenizer>{};
static std::vector<ska::Token> tokens;
static auto readerI = std::unique_ptr<ska::ScriptAST>{};
static auto scriptCacheI = ska::ScriptCacheAST{};
static auto typeCrosserI = ska::TypeCrosser{};

static BytecodeGeneratorDataTestContainer ASTFromInputBytecodeGeneratorNoParse(const std::string& input) {
	BytecodeGeneratorDataTestContainer data;
	tokenizer = std::make_unique<ska::Tokenizer>(reservedKeywords, input);
	tokens = tokenizer->tokenize();
	scriptCacheI.clear();
	readerI = std::make_unique<ska::ScriptAST>(scriptCacheI, "main", tokens);

	data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosserI);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosserI);
	data.generator = std::make_unique<ska::bytecode::Generator>(reservedKeywords);
	return data;
}

static std::pair<ska::bytecode::ScriptGenerationService, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = ASTFromInputBytecodeGeneratorNoParse(input);
	readerI->parse(*data.parser);
	return std::make_pair<ska::bytecode::ScriptGenerationService, BytecodeGeneratorDataTestContainer>(ska::bytecode::ScriptGenerationService{0, *readerI }, std::move(data));
}

struct BytecodePart {
	ska::bytecode::Command command;
	std::string dest;
	std::string left;
	std::string right;
};

static void BytecodeCompare(const ska::bytecode::GenerationOutput& result, std::vector<BytecodePart> expected) {
	auto index = std::size_t {0};
	// main script is always of id "0"
	const auto& scriptResult = result.generated(0);
	CHECK(scriptResult.size() == expected.size());
	for(const auto& r : scriptResult) {
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

TEST_CASE("[BytecodeGenerator] import ") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("var Player = import \"" SKALANG_TEST_DIR "/src/resources/play\";");
	auto res = data.generator->generate(data.storage, std::move(astPtr));
	CHECK(res.size() == 3);
	BytecodeCompare(res, {
		{ Command::SCRIPT, "R0", "1" },
		{ Command::MOV, "V0", "R0" }
	});
}

TEST_CASE("[BytecodeGenerator] Outside script from file (import) and use") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(
		"var Character184 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var player = Character184.build(\"Player\");"
		"var enemy = Character184.default; enemy.age;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));
	//CHECK(res.nodeval<int>() == 10);
}

TEST_CASE("[BytecodeGenerator] Use 2x same script and modifying a value in first import var : should modify also value in second import var") {
	constexpr auto progStr =
		"var Character91 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"var Character92 = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
		"Character91.default.age = 123;"
		"var t = Character92.default.age;";
	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto gen = data.generator->generate(data.storage, std::move(astPtr));
}

TEST_CASE("[BytecodeGenerator] C++ 1 script-function binding") {
	/*auto [astPtr, data] = ASTFromInputBytecodeGenerator("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto res = data.generator->generate(std::move(astPtr));

	BytecodeCompare(res, {
	});*/

	/*
	auto data = ASTFromInputBytecodeGeneratorNoParse("var User77 = import \"bind:binding\"; User77.funcTest(14, \"titito\");");
	auto test = 0;
	auto testStr = std::string{ "" };
	auto function = std::function<int(ska::Script&, int, ska::StringShared)>(
		[&](ska::Script&, int toto, ska::StringShared titi) -> int {
		test = toto;
		testStr = std::move(*titi);
		return 0;
	});


	auto scriptBinding = ska::ScriptBridge{ scriptCacheIS, "binding", *data.typeBuilder, *data.symbolsTypeUpdater, reservedKeywordsS };
	scriptBinding.bindFunction("funcTest", std::move(function));
	scriptBinding.buildFunctions();

	readerI->parse(*data.parser);
	auto generated = data.generator->generate(ska::bytecode::Script{ *readerI });
	*/
}

