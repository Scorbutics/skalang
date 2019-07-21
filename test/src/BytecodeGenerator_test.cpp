#include <doctest.h>
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

static auto ASTFromInputBytecodeGenerator(const std::string& input, BytecodeGeneratorDataTestContainer& data) {
	ASTFromInputBytecodeGeneratorNoParse(input, data);
	readerI->parse(*data.parser);
	return ska::BytecodeScript{ *readerI };
}

TEST_CASE("[BytecodeGenerator]") {
	BytecodeGeneratorDataTestContainer data;

	SUBCASE("OK") {
		SUBCASE("literal") {
			auto astPtr = ASTFromInputBytecodeGenerator("4;", data);
			auto res = data.generator->generate(astPtr);
		}

		SUBCASE("var declaration") {
			auto astPtr = ASTFromInputBytecodeGenerator("var toto = 4;", data);
			auto res = data.generator->generate(astPtr);			
		}
		
		SUBCASE("Basic Maths with var") {
			auto astPtr = ASTFromInputBytecodeGenerator("var toto = 4; toto; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.generator->generate(astPtr);
		}
	}
}
