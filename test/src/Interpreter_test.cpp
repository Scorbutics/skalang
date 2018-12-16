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
		SUBCASE("") {
			auto astPtr = ASTFromInputSemanticTCInterpreter("(4 * 5) + 2 * (3 + 4 - 1) + 1;", data);
			data.interpreter->interpret(*astPtr);
		}
	}
		
}
