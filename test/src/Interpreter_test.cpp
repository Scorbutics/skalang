#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/Parser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticTCInterpreter(const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };

	data.parser = std::make_unique<ska::Parser> ( reservedKeywords, reader );
    data.symbols = std::make_unique<ska::SymbolTable> (*data.parser);
    data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, *data.symbols);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser, *data.symbols);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, *data.symbols);
	data.interpreter = std::make_unique<ska::Interpreter>(*data.symbols);
    return data.parser->parse();
}

TEST_CASE("[Interpreter]") {
    DataTestContainer data;
    
	SUBCASE("OK") {
		SUBCASE("Basic Maths") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("(4 * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			data.interpreter->interpret(*astPtr);
            CHECK(std::get<int>((*astPtr)[0].value()) == 42);
		}
		SUBCASE("Basic Maths with var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<int>((*astPtr)[0].value()) == 4);
			CHECK(std::get<int>((*astPtr)[1].value()) == 42);
		}

		SUBCASE("Var declared") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 14;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<int>((*astPtr)[0].value()) == 14);
		}

		SUBCASE("Var reaffected") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto = 25;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<int>((*astPtr)[0].value()) == 4);
			CHECK(std::get<int>((*astPtr)[1].value()) == 25);
		}

		SUBCASE("Var reaffected using the same var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto = toto * 2;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<int>((*astPtr)[0].value()) == 4);
			CHECK(std::get<int>((*astPtr)[1].value()) == 8);
		}

		SUBCASE("Var reaffected using the same var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\"; toto = toto * 2;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<std::string>((*astPtr)[0].value()) == "ti");
			CHECK(std::get<std::string>((*astPtr)[1].value()) == "titi");
		}

		SUBCASE("Var reaffected string") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\" * 2 + \"to\";", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<std::string>((*astPtr)[0].value()) == "titito");
		}
		
		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\";", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<std::string>((*astPtr)[0].value()) == "2ti");
		}

		SUBCASE("Var reaffected string with number * 2") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\" * 2;", data);
			data.interpreter->interpret(*astPtr);
			CHECK(std::get<std::string>((*astPtr)[0].value()) == "2titi");
		}

        SUBCASE("If block") {
            auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } toto;", data);
            data.interpreter->interpret(*astPtr);
            CHECK(std::get<int>((*astPtr)[2].value()) == 123);
        }

	}
		
}
