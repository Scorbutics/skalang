#include <doctest.h>
#include "AST.h"
#include "Parser.h"
#include "TokenReader.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Operator.h"

TEST_CASE("test") {
	const auto inputStr = std::string("for(var i = 0; i < 5; i++) { lol(\"mdr\", 12); }");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();
	auto reader = ska::TokenReader { tokens };
	auto p = ska::Parser { keywords, reader };
	auto ast = p.parse();
	
	CHECK(ast->size() == 1);

	auto& tree = (*ast)[0];
	
	CHECK(tree.size() == 4);
	const auto& declaration = tree[0];
	CHECK(declaration.op == ska::Operator::VARIABLE_DECLARATION);
	CHECK(declaration.size() == 1);
	CHECK(declaration.token == ska::Token { "i", ska::TokenType::IDENTIFIER });
	CHECK(declaration[0].token == ska::Token { "0", ska::TokenType::DIGIT });

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
	const auto& functionCallStatement = loopStatementBody[0];

	CHECK(functionCallStatement.op == ska::Operator::FUNCTION_CALL);
	CHECK(functionCallStatement.token == ska::Token { "lol", ska::TokenType::IDENTIFIER });
	CHECK(functionCallStatement.size() == 2);
	CHECK(functionCallStatement[0].token == ska::Token { "\"mdr\"", ska::TokenType::STRING });
	CHECK(functionCallStatement[1].token == ska::Token { "12", ska::TokenType::DIGIT });
}

std::unique_ptr<ska::ASTNode> ASTFromInput(const std::string& input, const ska::ReservedKeywordsPool& keywords) {
	auto t = ska::Tokenizer {keywords, input};
	auto tokens = t.tokenize();
	auto reader = ska::TokenReader { tokens };
	auto p = ska::Parser { keywords, reader };
	return p.parse();
}

TEST_CASE("Block") {
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("Empty block statement") {
		auto astPtr = ASTFromInput("{}", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BLOCK);
		CHECK(ast.size() == 0);
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput("{ test; }", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BLOCK);
		CHECK(ast.size() == 1);
		CHECK(ast[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput("{ test; titi; }", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BLOCK);
		CHECK(ast.size() == 2);
		CHECK(ast[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
		CHECK(ast[1].token == ska::Token { "titi", ska::TokenType::IDENTIFIER});
	}

	SUBCASE("1 statement, then a block statement") {
		auto astPtr = ASTFromInput("tititi; { test; titi; }", keywords);
		auto& ast = *astPtr;
		CHECK(ast.op == ska::Operator::BLOCK);
		CHECK(ast.size() == 2);
		CHECK(ast[1].op == ska::Operator::BLOCK);
		CHECK(ast[1].size() == 2);
	}
}

TEST_CASE("for") {
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("All empty") {
		const auto astPtr = ASTFromInput("for(;;);", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::FOR_LOOP);
		CHECK(ast.size() == 3);
		CHECK(ast[0].empty());
		CHECK(ast[1].empty());
		CHECK(ast[2].empty());

	}
}

TEST_CASE("If keyword pattern") {
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("If only with cond and block statement") {
		auto astPtr = ASTFromInput("if (test) {}", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::IF);
		CHECK(ast.size() == 2);
		CHECK(ast[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
		CHECK(ast[1].op == ska::Operator::BLOCK);
	}
}

TEST_CASE("function") {
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("with 2 arguments built-in types and no return type") {
		auto astPtr = ASTFromInput("var f = function(titi:int, toto:string) : var { };", keywords);
		auto& ast = (*astPtr)[0];
        CHECK(ast.op == ska::Operator::VARIABLE_DECLARATION);
        const auto& astFunc = ast[0];
        CHECK(astFunc.op == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc.size() == 3);
		//CHECK(ast[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
		
	}
}

TEST_CASE("User defined object") {
    const auto keywords = ska::ReservedKeywordsPool {};
	
    SUBCASE("constructor with 1 parameter") {
		auto astPtr = ASTFromInput("var Joueur = function(nom:string) : var { return { nom : nom }; }; ff var joueur1 = Joueur(\"joueur 1\"); joueur1.nom;", keywords);
		auto& ast = (*astPtr)[0];
        CHECK(ast.op == ska::Operator::VARIABLE_DECLARATION);
        const auto& astFunc = ast[0];
        CHECK(astFunc.op == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc.size() == 2);
		//TODO suite des verifs
	}

}

TEST_CASE("Expression and priorities") {
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("Simple mul") {
		auto astPtr = ASTFromInput("5 * 2;", keywords);
    auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "*", ska::TokenType::SYMBOL });
		CHECK(ast[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(ast[1].token == ska::Token { "2", ska::TokenType::DIGIT });
	}

	SUBCASE("Syntax error : not an expression") {
		auto toCheck = true;
		try {
			ASTFromInput("5 * 2", keywords);
		} catch (std::exception& e) {
			toCheck = false;
		}
		CHECK(!toCheck);
	}
	
	SUBCASE("Simple div") {
		auto astPtr = ASTFromInput("5 / 2;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "/", ska::TokenType::SYMBOL });
		CHECK(ast[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(ast[1].token == ska::Token { "2", ska::TokenType::DIGIT });
	}
	
	SUBCASE("Simple add") {
		auto astPtr = ASTFromInput("5 + 2;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "+", ska::TokenType::SYMBOL });
		CHECK(ast[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(ast[1].token == ska::Token { "2", ska::TokenType::DIGIT });
	}

	SUBCASE("Simple sub") {
		auto astPtr = ASTFromInput("5 - 2;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "-", ska::TokenType::SYMBOL });
		CHECK(ast[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(ast[1].token == ska::Token { "2", ska::TokenType::DIGIT });
	}

	SUBCASE("Priorization with mul before add") {
		auto astPtr = ASTFromInput("5 * 2 + 4;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "+", ska::TokenType::SYMBOL });
		CHECK(ast[1].token == ska::Token { "4", ska::TokenType::DIGIT });
		CHECK(ast[0].op == ska::Operator::BINARY);
		const auto& innerOp = ast[0];
		CHECK(innerOp[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(innerOp[1].token == ska::Token { "2", ska::TokenType::DIGIT });
		CHECK(innerOp.token == ska::Token { "*", ska::TokenType::SYMBOL });
	}

	SUBCASE("Priorization with mul after add") {
		auto astPtr = ASTFromInput("5 + 2 * 4;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "+", ska::TokenType::SYMBOL });
		CHECK(ast[1].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(ast[0].op == ska::Operator::BINARY);
		const auto& innerOp = ast[0];
		CHECK(innerOp[0].token == ska::Token { "2", ska::TokenType::DIGIT });
		CHECK(innerOp[1].token == ska::Token { "4", ska::TokenType::DIGIT });	
		CHECK(innerOp.token == ska::Token { "*", ska::TokenType::SYMBOL });
	}


	SUBCASE("Priorization with mul after add with parenthesis") {
		auto astPtr = ASTFromInput("(5 + 2) * 4;", keywords);
		auto& ast = (*astPtr)[0];
		CHECK(ast.op == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.token == ska::Token { "*", ska::TokenType::SYMBOL });
		CHECK(ast[1].token == ska::Token { "4", ska::TokenType::DIGIT });
		CHECK(ast[0].op == ska::Operator::BINARY);
		const auto& innerOp = ast[0];
		CHECK(innerOp[0].token == ska::Token { "5", ska::TokenType::DIGIT });
		CHECK(innerOp[1].token == ska::Token { "2", ska::TokenType::DIGIT });	
		CHECK(innerOp.token == ska::Token { "+", ska::TokenType::SYMBOL });
	}
	
}
