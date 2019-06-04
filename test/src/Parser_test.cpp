#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "NodeValue/Operator.h"
#include "Interpreter/Value/Script.h"

TEST_CASE("[Parser]") {
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	const auto inputStr = std::string("for(var i = 0; i < 5; i++) { lol; \"mdr\"; 12; }");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();
	auto reader = ska::Script { scriptCache, "main", tokens };
	auto p = ska::StatementParser{ keywords };
	reader.parse(p, false);
	auto ast = &reader.rootNode();
	
	CHECK(ast->size() == 1);

	auto& tree = (*ast)[0];
	
	CHECK(tree.size() == 4);
	const auto& declaration = tree[0];
	CHECK(declaration.op() == ska::Operator::VARIABLE_DECLARATION);
	CHECK(declaration.size() == 1);
	CHECK(declaration.has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	CHECK(declaration[0].has(ska::Token { "0", ska::TokenType::DIGIT, {} }));

	const auto& checkStatement = tree[1];
	CHECK(checkStatement.op() == ska::Operator::BINARY);
	CHECK(checkStatement.size() == 2);
	CHECK(checkStatement[0].has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	CHECK(checkStatement.has(ska::Token { "<", ska::TokenType::SYMBOL, {} }));
	CHECK(checkStatement[1].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));

	const auto& incrementStatement = tree[2];
	CHECK(incrementStatement.op() == ska::Operator::UNARY);
	CHECK(incrementStatement.size() == 1);
	CHECK(incrementStatement[0].has(ska::Token { "i", ska::TokenType::IDENTIFIER, {} }));
	
	const auto& loopStatementBody = tree[3];
	CHECK(loopStatementBody.op() == ska::Operator::BLOCK);
	CHECK(loopStatementBody.size() == 3);
	const auto& functionCallStatement = loopStatementBody[0];
}

ska::Script ASTFromInput(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, const ska::ReservedKeywordsPool& keywords) {
	auto t = ska::Tokenizer {keywords, input};
	auto tokens = t.tokenize();
	auto reader = ska::Script { scriptCache, "main", tokens };
	auto p = ska::StatementParser{ keywords };
	reader.parse(p, false);
    return reader;
}

TEST_CASE("Block") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	SUBCASE("Empty block statement") {
		auto astPtr = ASTFromInput(scriptCache, "{}", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BLOCK);
		CHECK(ast.size() == 0);
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput(scriptCache, "{ test; }", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BLOCK);
		CHECK(ast.size() == 1);
		CHECK(ast[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
	}

	SUBCASE("1 statement block statement") {
		auto astPtr = ASTFromInput(scriptCache, "{ test; titi; }", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BLOCK);
		CHECK(ast.size() == 2);
		CHECK(ast[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
		CHECK(ast[1].has(ska::Token { "titi", ska::TokenType::IDENTIFIER, {}}));
	}

	SUBCASE("1 statement, then a block statement") {
		auto& ast = ASTFromInput(scriptCache, "tititi; { test; titi; }", keywords).rootNode();

		CHECK(ast.op() == ska::Operator::BLOCK);
		CHECK(ast.size() == 2);
		CHECK(ast[1].op() == ska::Operator::BLOCK);
		CHECK(ast[1].size() == 2);
	}
}

TEST_CASE("for") {
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	const auto keywords = ska::ReservedKeywordsPool {};
	SUBCASE("All empty") {
		auto astPtr = ASTFromInput(scriptCache, "for(;;);", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::FOR_LOOP);
		CHECK(ast.size() == 3);
		CHECK(ast[0].logicalEmpty());
		CHECK(ast[1].logicalEmpty());
		CHECK(ast[2].logicalEmpty());

	}
}

TEST_CASE("booleans") {
	const auto keywords = ska::ReservedKeywordsPool{};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	SUBCASE("true") {
		auto astPtr = ASTFromInput(scriptCache, "true;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::LITERAL);
		CHECK(ast.has(ska::Token{ "true", ska::TokenType::BOOLEAN, {}}));
	}

	SUBCASE("false") {
		auto astPtr = ASTFromInput(scriptCache, "false;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::LITERAL);
		CHECK(ast.has(ska::Token{ "false", ska::TokenType::BOOLEAN, {}}));
	}
}

TEST_CASE("If keyword pattern") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	SUBCASE("If only with cond and block statement") {
		auto astPtr = ASTFromInput(scriptCache, "if (test) {}", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::IF);
		CHECK(ast.size() == 2);
		CHECK(ast[0].has(ska::Token { "test", ska::TokenType::IDENTIFIER, {}}));
		CHECK(ast[1].op() == ska::Operator::BLOCK);
	}
}

TEST_CASE("function") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	SUBCASE("with 2 arguments built-in types and no return type") {
		auto astPtr = ASTFromInput(scriptCache, "var f = function(titi:int, toto:string) { };", keywords);
		auto& ast = astPtr.rootNode()[0];
        CHECK(ast.op() == ska::Operator::VARIABLE_DECLARATION);
        const auto& astFunc133 = ast[0];
        CHECK(astFunc133.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc133.size() == 2);
		
        CHECK(astFunc133[0].size() == 3);
        CHECK(astFunc133[1].size() == 0);
        //CHECK(ast[0].token == ska::Token { "test", ska::TokenType::IDENTIFIER});
		
	}
	//TODO rework : doesn't properly work (doesn't detect a good function returning type)
	SUBCASE("with 2 return placements (early return support)") {
		auto astPtr = ASTFromInput(scriptCache, "var f_parser154 = function(titi:int) : int { if(titi == 0) { return 1; } return 0; }; var int_parser154 = f_parser154(1);", keywords);
		auto& ast = astPtr.rootNode()[0];
        CHECK(ast.op() == ska::Operator::VARIABLE_DECLARATION);
        const auto& astFunc157 = ast[0];
        CHECK(astFunc157.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc157.size() == 2);
	}
}

TEST_CASE("User defined object") {
    const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	
    SUBCASE("constructor with 1 parameter") {
        
        auto astPtr = ASTFromInput(scriptCache, "var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.nom;", keywords);
		CHECK(astPtr.rootNode().size() == 3);
        CHECK(astPtr.rootNode().op() == ska::Operator::BLOCK);
        
        auto& varJoueurNode = astPtr.rootNode()[0];
        CHECK(varJoueurNode.size() == 1);
        CHECK(varJoueurNode.op() == ska::Operator::VARIABLE_DECLARATION);
        const auto& astFunc154 = varJoueurNode[0];
        CHECK(astFunc154.op() == ska::Operator::FUNCTION_DECLARATION);
		CHECK(astFunc154.size() == 2);
        const auto& astFuncParameters154 = astFunc154[0];
        CHECK(astFuncParameters154.size() == 2);
    
        //Checks the parameter name and type
        CHECK(astFuncParameters154[0].has(ska::Token { "nom", ska::TokenType::IDENTIFIER, {}})); 
        CHECK(astFuncParameters154[0].size() == 1);
		CHECK(astFuncParameters154[0][0].size() == 3);
        CHECK(astFuncParameters154[0][0][0].has(keywords.pattern<ska::TokenGrammar::STRING>()));

        //Checks the return type
        CHECK(astFuncParameters154[1][0].has(keywords.pattern<ska::TokenGrammar::VARIABLE>()));

        //Checks the function body
        CHECK(astFunc154[1].size() == 1);
        
        const auto& userDefinedObjectNode = astFunc154[1][0][0];
		CHECK(astFunc154[1][0].op() == ska::Operator::RETURN);
        CHECK(userDefinedObjectNode.op() == ska::Operator::USER_DEFINED_OBJECT);
        CHECK(userDefinedObjectNode.size() == 1);

        const auto& returnNomNode = userDefinedObjectNode[0];
        CHECK(returnNomNode.size() == 1);
        CHECK(returnNomNode.has(ska::Token { "nom", ska::TokenType::IDENTIFIER, {}}));
        
        //Checks the variable declaration and the function call
        const auto& varJoueur1Node = astPtr.rootNode()[1];
        CHECK(varJoueur1Node.op() == ska::Operator::VARIABLE_DECLARATION);
        CHECK(varJoueur1Node.has(ska::Token { "joueur1", ska::TokenType::IDENTIFIER, {}} ));

        //Checks the field access
        const auto& nomJoueur1FieldNode = astPtr.rootNode()[2];
        CHECK(nomJoueur1FieldNode.op() == ska::Operator::FIELD_ACCESS);
		CHECK(nomJoueur1FieldNode.size() == 2);
		CHECK(nomJoueur1FieldNode[0].has(ska::Token { "joueur1", ska::TokenType::IDENTIFIER, {}} ));
		CHECK(nomJoueur1FieldNode[1].has(ska::Token{ "nom", ska::TokenType::IDENTIFIER, {}}));

	}

}

TEST_CASE("Expression and priorities") {
	const auto keywords = ska::ReservedKeywordsPool {};
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	SUBCASE("Simple mul") {
		auto astPtr = ASTFromInput(scriptCache, "5 * 2;", keywords);
        auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "*", ska::TokenType::SYMBOL, {}}));
		CHECK(ast[0].has(ska::Token { "5", ska::TokenType::DIGIT, {}}));
		CHECK(ast[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Syntax error : not an expression") {
		auto toCheck = true;
		try {
			ASTFromInput(scriptCache, "5 * 2", keywords);
		} catch (std::exception& e) {
			toCheck = false;
		}
		CHECK(!toCheck);
	}
	
    SUBCASE("Syntax error : no existing operator") {
        try {
            ASTFromInput(scriptCache, "5 ' 3;", keywords);
            CHECK(false);
        } catch(std::exception& e) {
            CHECK(true);
        }
    }

	SUBCASE("Simple div") {
		auto astPtr = ASTFromInput(scriptCache, "5 / 2;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "/", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(ast[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}
	
	SUBCASE("Simple add") {
		auto astPtr = ASTFromInput(scriptCache, "5 + 2;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(ast[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Simple sub") {
		auto astPtr = ASTFromInput(scriptCache, "5 - 2;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "-", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(ast[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
	}

	SUBCASE("Priorization with mul before add") {
		auto astPtr = ASTFromInput(scriptCache, "5 * 2 + 4;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));
		CHECK(ast[0].op() == ska::Operator::BINARY);
		const auto& innerOp = ast[0];
		CHECK(innerOp[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
	}

	SUBCASE("Priorization with mul after add") {
		auto astPtr = ASTFromInput(scriptCache, "5 + 2 * 4;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(ast[1].op() == ska::Operator::BINARY);
		const auto& innerOp = ast[1];
		CHECK(innerOp[0].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));	
		CHECK(innerOp.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
	}


	SUBCASE("Priorization with mul after add with parenthesis") {
		auto astPtr = ASTFromInput(scriptCache, "(5 + 2) * 4;", keywords);
		auto& ast = astPtr.rootNode()[0];
		CHECK(ast.op() == ska::Operator::BINARY);
		CHECK(ast.size() == 2);
		CHECK(ast.has(ska::Token { "*", ska::TokenType::SYMBOL, {} }));
		CHECK(ast[1].has(ska::Token { "4", ska::TokenType::DIGIT, {} }));
		CHECK(ast[0].op() == ska::Operator::BINARY);
		const auto& innerOp = ast[0];
		CHECK(innerOp[0].has(ska::Token { "5", ska::TokenType::DIGIT, {} }));
		CHECK(innerOp[1].has(ska::Token { "2", ska::TokenType::DIGIT, {} }));	
		CHECK(innerOp.has(ska::Token { "+", ska::TokenType::SYMBOL, {} }));
	}

	//TODO Unit tests on tokens position in script
}
