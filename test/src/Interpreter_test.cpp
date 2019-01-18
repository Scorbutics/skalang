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
			auto res = data.interpreter->interpret(*astPtr);
            CHECK(res.asRvalue().nodeval<int>() == 42);
		}
		SUBCASE("Basic Maths with var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; (toto * 5) + 2 * (3 + 4 - 1) + 1 + 9;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 42);
		}

		SUBCASE("Var declared") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 14; toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 14);
		}

		SUBCASE("Var reaffected") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = 25;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 25);
		}

		SUBCASE("Var reaffected using the same var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 4; toto; toto = toto * 2;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 8);
		}

		SUBCASE("Var reaffected using the same var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\"; toto; toto = toto * 2;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval < std::string > () == "titi");
		}

		SUBCASE("Var reaffected string") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = \"ti\" * 2 + \"to\"; toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval < std::string >() == "titito");
		}
		
		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval < std::string >() == "2ti");
		}

		SUBCASE("Var reaffected string with number * 2") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\" * 2; toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval < std::string >() == "2titi");
		}

        SUBCASE("If block true") {
            auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 123);
        }

		SUBCASE("If block false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 1);
		}

		SUBCASE("If block pure true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 0; if(true) { toto = 123; } toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 123);
		}

		SUBCASE("If Else block if false") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 12) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 24);
		}

		SUBCASE("If Else block if true") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 1; if(toto + 1 == 2) { toto = 123; } else { toto = 24; } toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 123);
		}

		SUBCASE("Var reaffected string with number") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = 2 + \"ti\"; toto;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval < std::string >() == "2ti");
		}

		SUBCASE("Array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1];", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 25);
		}

		SUBCASE("Assigning a cell of an array of int") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var toto = [14, 25, 13, 2]; toto[1] = 226; toto[1];", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 226);
		}

		SUBCASE("Basic function") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function() : int { return 3; }; var titi = totoFunc(); titi;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 3);
		}

		SUBCASE("Function 1 parameter") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 10);
		}

		SUBCASE("Function 1 parameter 2x calls") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var totoFunc = function(value: int) : int { return value + 3; }; var titi = totoFunc(7); titi = totoFunc(17);", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 20);
		}

		SUBCASE("Function 1 parameter using a parent scope var") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("var test = 20; var totoFunc = function(value: int) : int { return test + value + 3; }; var titi = totoFunc(7); titi;", data);
			auto res = data.interpreter->interpret(*astPtr);
			CHECK(res.asRvalue().nodeval<int>() == 30);
		}
	}
		
}
