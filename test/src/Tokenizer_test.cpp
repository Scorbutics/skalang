#include <doctest.h>
#include "LoggerConfigLang.h"
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

TEST_CASE("Symbol by symbol") {
	const auto keywords = ska::ReservedKeywordsPool{};
	
	SUBCASE("for") {
		const auto input = std::string("for");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FOR>());
	}

	SUBCASE("var") {
		const auto input = std::string("var");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::VARIABLE>());
	}

	SUBCASE("int") {
		const auto input = std::string("int");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::INT>());
	}
	
	SUBCASE("float") {
		const auto input = std::string("float");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FLOAT>());
	}
	
	SUBCASE("string") {
		const auto input = std::string("string");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STRING>());
	}
	
	SUBCASE("if") {
		const auto input = std::string("if");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::IF>());
	}

	SUBCASE("else") {
		const auto input = std::string("else");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::ELSE>());
	}

	SUBCASE("return") {
		const auto input = std::string("return");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::RETURN>());
	}

	SUBCASE("function") {
		const auto input = std::string("function");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FUNCTION>());
	}

	SUBCASE("any identifier") {
		const auto input = std::string("testId");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token{"testId", ska::TokenType::IDENTIFIER});
	}

	SUBCASE("end statement symbol") {
		const auto input = std::string(";");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	}

	SUBCASE("block begin symbol") {
		const auto input = std::string("{");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::BLOCK_BEGIN>());
	}

	SUBCASE("block end symbol") {
		const auto input = std::string("}");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::BLOCK_END>());
	}

	SUBCASE("parenthesis begin") {
		const auto input = std::string("(");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	}

	SUBCASE("parenthesis end") {
		const auto input = std::string(")");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	}

	SUBCASE("random symbol : equal sign affectation") {
		const auto input = std::string("=");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"=", ska::TokenType::SYMBOL});
	}

	SUBCASE("integer") {
		const auto input = std::string("234");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "234", ska::TokenType::DIGIT});
	}
	
	SUBCASE("real number") {
		const auto input = std::string("32.1");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"32.1", ska::TokenType::DIGIT});
	}

	SUBCASE("Dot symbol (for method call)") {
		const auto input = std::string(".");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::METHOD_CALL_OPERATOR>());
	}

	SUBCASE("string") {
		const auto input = std::string("\"tettttt\"");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"\"tettttt\"", ska::TokenType::STRING});
	}

	SUBCASE("Full method call 0 args") {
		const auto input = std::string("test.call()");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 5);
		CHECK(tokens[0] == ska::Token { "test", ska::TokenType::IDENTIFIER});
		CHECK(tokens[1] == keywords.pattern<ska::TokenGrammar::METHOD_CALL_OPERATOR>());
		CHECK(tokens[2] == ska::Token { "call", ska::TokenType::IDENTIFIER});
		CHECK(tokens[3] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
		CHECK(tokens[4] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	}

	SUBCASE("only spaces") {
		const auto input = std::string("  \t \n ");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.empty());
	}

	SUBCASE("spaces with not space in middle") {
		const auto input = std::string("  \t o \n ");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "o", ska::TokenType::IDENTIFIER});
	}
	
	SUBCASE("string with noise around") {
		const auto input = std::string(" 13.23  \"testString\" var");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 3);
		CHECK(tokens[0] == ska::Token { "13.23", ska::TokenType::DIGIT });
		CHECK(tokens[1] == ska::Token { "\"testString\"", ska::TokenType::STRING});
		CHECK(tokens[2] == keywords.pattern<ska::TokenGrammar::VARIABLE>());
	}

	SUBCASE("string with reserved symbols and keywords") {
		const auto input = std::string("\"var i = 0; {}\"");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "\"var i = 0; {}\"", ska::TokenType::STRING});
	}

	SUBCASE("symbols in queue") {
		const auto input = std::string(";;++--");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();
		
		CHECK(tokens.size() == 4);
		CHECK(tokens[0] == ska::Token {";", ska::TokenType::SYMBOL});
		CHECK(tokens[1] == ska::Token {";", ska::TokenType::SYMBOL});
		CHECK(tokens[2] == ska::Token {"++", ska::TokenType::SYMBOL});
		CHECK(tokens[3] == ska::Token {"--", ska::TokenType::SYMBOL});
	}

	SUBCASE("double symbols tokens") {
		const auto input = std::string("--");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"--", ska::TokenType::SYMBOL} );
	}
}

