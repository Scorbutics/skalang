#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include "Tokenizer.h"
#include "ReservedKeywordsPool.h"

TEST_CASE("test") {
	const auto inputStr = std::string("for(var i = 0; i < 5; i++) { lol(\"mdr\", 12); }");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();

	CHECK(tokens.size() == 23);
	CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FOR>());
	CHECK(tokens[1] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	CHECK(tokens[2] == keywords.pattern<ska::TokenGrammar::VARIABLE>());
	CHECK(tokens[3] == ska::Token{"i", ska::TokenType::IDENTIFIER});
	CHECK(tokens[4] == ska::Token{"=", ska::TokenType::SYMBOL});
	CHECK(tokens[5] == ska::Token{"0", ska::TokenType::DIGIT});
	CHECK(tokens[6] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	CHECK(tokens[7] == ska::Token{"i", ska::TokenType::IDENTIFIER});
	CHECK(tokens[8] == ska::Token{"<", ska::TokenType::SYMBOL});
	CHECK(tokens[9] == ska::Token{"5", ska::TokenType::DIGIT});
	CHECK(tokens[10] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	CHECK(tokens[11] == ska::Token{"i", ska::TokenType::IDENTIFIER});
	CHECK(tokens[12] == ska::Token{"++", ska::TokenType::SYMBOL});
	CHECK(tokens[13] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	CHECK(tokens[14] == keywords.pattern<ska::TokenGrammar::BLOCK_BEGIN>());
	CHECK(tokens[15] == ska::Token { "lol", ska::TokenType::IDENTIFIER});
	CHECK(tokens[16] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	CHECK(tokens[17] == ska::Token { "\"mdr\"", ska::TokenType::STRING});
	CHECK(tokens[18] == keywords.pattern<ska::TokenGrammar::ARGUMENT_DELIMITER>());
	CHECK(tokens[19] == ska::Token { "12", ska::TokenType::DIGIT});
	CHECK(tokens[20] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	CHECK(tokens[21] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	CHECK(tokens[22] == keywords.pattern<ska::TokenGrammar::BLOCK_END>());
}
