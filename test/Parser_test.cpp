#include <doctest.h>
#include "Parser.h"
#include "TokenReader.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"

TEST_CASE("test") {
	const auto inputStr = std::string("for(var i = 0; i < 5; i++) { lol(\"mdr\", 12); }");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();
	auto reader = ska::TokenReader { tokens };
	auto p = ska::Parser { keywords, reader };
	auto ast = p.parse();
	
	std::cout << ast.first->asString() << std::endl;
}
