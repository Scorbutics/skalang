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

static std::pair<ska::bytecode::ScriptGenerationService, BytecodeGeneratorDataTestContainer> ASTFromInputBytecodeGenerator(const std::string& input) {
	auto data = BytecodeGeneratorDataTestContainer{};
	ASTFromInputBytecodeGeneratorNoParse(input, data);
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
	const auto& scriptResult = result.back();
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

TEST_CASE("[BytecodeGenerator] empty if") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) {}");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "1", "0" }
	});
}

TEST_CASE("[BytecodeGenerator] if with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( true ) { var toto = 5; }");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_NIF, "1", "1" },
		{ Command::MOV, "V0", "5" }
	});
}

TEST_CASE("[BytecodeGenerator] if else with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("if( 3 > 2 ) { var toto = 1 + 3; } else { var toto = 4 + 2 + 1; var tt = toto; } var tete = 444;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::SUB_I, "R0", "3", "2" },
		{ Command::TEST_G, "R0", "R0" },
		{ Command::JUMP_NIF, "R0", "3" },
		{ Command::ADD_I, "R1", "1", "3" },
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "4" },
		{ Command::ADD_I, "R2", "2", "1" },
		{ Command::ADD_I, "R3", "4", "R2" },
		{ Command::MOV, "V1", "R3" },
		{ Command::MOV, "V2", "V1" },
		{ Command::MOV, "V3", "444" }
	});

}

TEST_CASE("[BytecodeGenerator] empty for") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(;;) {}");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		{ Command::JUMP_REL, "-1" }
	});
}

TEST_CASE("[BytecodeGenerator] for without body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(var i = 0; i < 10; i = i + 1);");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },
		
		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "R0", "3" },
		
		// Body part
		
		// Increment part
		{ Command::ADD_I, "R1", "V0", "1"},
		{ Command::MOV, "V0", "R1" },
		{ Command::JUMP_REL, "-6" }
	});
}

TEST_CASE("[BytecodeGenerator] for with body") {
	auto [astPtr, data] = ASTFromInputBytecodeGenerator("for(var i = 0; i < 10; i = i + 1) { var toto = 123; toto + i; } var test = 1234;");
	auto res = data.generator->generate(data.storage, std::move(astPtr));

	BytecodeCompare(res, {
		// Initialization part
		{ Command::MOV, "V0", "0" },

		// Check part
		{ Command::SUB_I, "R0", "V0", "10" },
		{ Command::TEST_L, "R0", "R0" },
		{ Command::JUMP_NIF, "R0", "5" },
		
		// Body part
		{ Command::MOV, "V1", "123" },
		{ Command::ADD_I, "R1", "V1", "V0" },

		// Increment part
		{ Command::ADD_I, "R2", "V0", "1"},
		{ Command::MOV, "V0", "R2" },
		{ Command::JUMP_REL, "-8" },

		// Post part
		{ Command::MOV, "V2", "1234"}
	});
}
