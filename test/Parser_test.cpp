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
	
	auto& tree = *ast.first;
	
	CHECK(tree.size() == 4);
	const auto& declaration = tree[0];
	CHECK(declaration.op == ska::Operator::VARIABLE_DECLARATION);
	CHECK(declaration.size() == 2);
	CHECK(declaration[0].token == ska::Token { "i", ska::TokenType::IDENTIFIER });
	CHECK(declaration[1].token == ska::Token { "0", ska::TokenType::DIGIT });

	const auto& checkStatement = tree[1];
	CHECK(checkStatement.op == ska::Operator::BINARY);
	CHECK(checkStatement.size() == 2);
	CHECK(checkStatement[0].token == ska::Token { "i", ska::TokenType::IDENTIFIER });
	CHECK(checkStatement.token == ska::Token { "<", ska::TokenType::SYMBOL });
	CHECK(checkStatement[1].token == ska::Token { "5", ska::TokenType::DIGIT });

	const auto& incrementStatement = tree[2];
	CHECK(incrementStatement.op == ska::Operator::UNARY);
	CHECK(incrementStatement.size() == 1);
	CHECK(incrementStatement[0].token == ska::Token { "i", ska::TokenType::IDENTIFIER });
	
	const auto& loopStatementBody = tree[3];
	CHECK(loopStatementBody.op == ska::Operator::BLOCK);
	CHECK(loopStatementBody.size() == 1);
	CHECK(loopStatementBody[0].op == ska::Operator::FUNCTION_CALL);
	const auto& functionCallStatement = loopStatementBody[0];

	CHECK(functionCallStatement.token == ska::Token { "lol", ska::TokenType::IDENTIFIER });
	CHECK(functionCallStatement.size() == 2);
	CHECK(functionCallStatement[0].token == ska::Token { "\"mdr\"", ska::TokenType::STRING });
	CHECK(functionCallStatement[1].token == ska::Token { "12", ska::TokenType::DIGIT });
	
}
