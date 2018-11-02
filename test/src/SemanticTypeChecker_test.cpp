#include <doctest.h>
#include "LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "TypeBuilder.h"

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticTC(const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };

	data.parser = std::make_unique<ska::Parser> ( reservedKeywords, reader );
    data.symbols = std::make_unique<ska::SymbolTable> (*data.parser);
    data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, *data.symbols);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser, *data.symbols);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, *data.symbols);
    return data.parser->parse();
}

TEST_CASE("[SemanticTypeChecker]") {
    DataTestContainer data;
    
    SUBCASE("OK") {
        SUBCASE("float x float") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 2.0; var toto = 5.2; titi * toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("int x string") {    
            auto astPtr = ASTFromInputSemanticTC("var titi = 2; var toto = \"test\"; titi * toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::STRING);
        }

        SUBCASE("int x float") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 2; var toto = 5.2; titi * toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("float x int") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 2.0; var toto = 5; titi * toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("string x int") {    
            auto astPtr = ASTFromInputSemanticTC("var titi = \"tititititit\"; var toto = 6; titi * toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::STRING);
        }

        SUBCASE("string = int") {    
            auto astPtr = ASTFromInputSemanticTC("var titi = \"strrrr\"; titi = 3;", data);  
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::STRING);
        }

        SUBCASE("float = string") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 0.1; titi = \"toto\";", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("float = string (variable)") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 0.1; var toto = \"toto\"; titi = toto;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 3);
            CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
        }
        
        SUBCASE("float = float") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 0.1; titi = 0.2;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("float = int") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 0.1; titi = 2;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
        }

        SUBCASE("int = float") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 1; titi = 0.2;", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::INT);
        }
        
        SUBCASE("int = string") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 7; titi = \"123\";", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::INT);
        }
        
        SUBCASE("float = string") {
            auto astPtr = ASTFromInputSemanticTC("var titi = 7.8; titi = \"123\";", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
        }
        
        SUBCASE("function call : string") {
            auto astPtr = ASTFromInputSemanticTC("var titi121 = function(test:string) {}; titi121(\"lol\");", data);
            auto& ast = (*astPtr);
            CHECK(ast.size() == 2);
            //std::cout << ast[1].type.value().asString() << std::endl;
            CHECK(ast[1].type() == ska::ExpressionType::VOID);
        }

        SUBCASE("Calling a function with a type convertible argument") {
            ASTFromInputSemanticTC("var titi129 = function(test:string) {}; titi129(23);", data);
        
        }

        SUBCASE("constructor with 1 parameter") {
            ASTFromInputSemanticTC("var Joueur1 = function(nomL:string) : var { return { nom : nomL }; }; var joueur1 = Joueur1(\"joueur 1\"); joueur1.nom;", data);
        
        }

		SUBCASE("constructor with 1 parameter same name") {
			ASTFromInputSemanticTC("var Joueur2 = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur2(\"joueur 1\"); joueur1.nom;", data);
		
        }

		SUBCASE("constructor complex with contained function NOT USING the current type...") {
			ASTFromInputSemanticTC(
			"var JoueurClass = function(nom:string) : var { "
				"var puissance = 10;"

				"var attaquer = function(cible:JoueurClass) {"
				"};"

				"return {"
					"nom: nom,"
					"puissance : puissance,"
					"pv : 100,"
					"attaquer : attaquer"
				"};"
			"};"
			"var joueur1 = JoueurClass(\"joueur1\");"
			"var joueur2 = JoueurClass(\"joueur2\");"
			"joueur1.attaquer(joueur2);"
				, data);
		}

		/*
		//TODO : très important, mais complexe à réaliser.
		SUBCASE("constructor complex with contained function USING the current type...") {
			ASTFromInputSemanticTC(
				"var Joueur = function(nom:string) : var { "
				"var puissance = 10;"

				"var attaquer = function(cible:Joueur) {"
					"cible.pv = cible.pv - puissance;"
				"};"

				"return {"
				"nom: nom,"
				"puissance : puissance,"
				"pv : 100,"
				"attaquer : attaquer"
				"};"
				"};"
				"var joueur1 = Joueur(\"joueur1\");"
				"var joueur2 = Joueur(\"joueur2\");"
				"joueur1.attaquer(joueur2);"
				, parser_test, table_test, type_test);
		}*/
    }

    SUBCASE("Fail") {
        SUBCASE("Because of non-matching type (variable then function)") {
            try {
                ASTFromInputSemanticTC("var i = 120; i = function() {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

        SUBCASE("Because of non-matching type (function then variable)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = 9;", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }
        
        SUBCASE("Reassigning function (function = function)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = function(ttt:string) {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

        SUBCASE("Calling a function with wrong type arguments") {
            try {
                ASTFromInputSemanticTC("var titi = function(test:function) {}; titi(23);", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

		SUBCASE("constructor with 1 parameter + bad field access") {
			try {
				ASTFromInputSemanticTC("var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.ttetetetet;", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(true);
			}
		}

		SUBCASE("constructor complex with contained function NOT USING the current type and calling member function with a wrong type...") {
			try {
				ASTFromInputSemanticTC(
				"var Joueur = function(nom:string) : var { "
					"var puissance = 10;"

					"var attaquer = function(cible:Joueur) {"
					"};"

					"return {"
						"nom: nom,"
						"puissance : puissance,"
						"pv : 100,"
						"attaquer : attaquer"
					"};"
				"};"
				"var joueur1 = Joueur(\"joueur1\");"
				"var joueur2 = Joueur(\"joueur2\");"
				"var toto = 1;"
				"joueur1.attaquer(toto);"
				, data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(true);
			}
		}
    }
}
