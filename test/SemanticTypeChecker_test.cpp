#include <doctest.h>
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::Parser>;
using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticTC(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test, SemanticTypeCheckerPtr& semanticTypeChecker_test) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	parser_test = std::make_unique<ska::Parser> ( reservedKeywords, reader );
	semanticTypeChecker_test = std::make_unique<ska::SemanticTypeChecker>(*parser_test);
    table_test = std::make_unique<ska::SymbolTable> (*parser_test);
	semanticTypeChecker_test->setSymbolTable(*table_test);
    return parser_test->parse();
}

TEST_CASE("Semantic type checker") {
    ParserPtr parser_test;
    SemanticTypeCheckerPtr type_test;
    SymbolTablePtr table_test;
    
    SUBCASE("OK") {
        SUBCASE("float x float") {
            ASTFromInputSemanticTC("var titi = 2.0; var toto = 5.2; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("int x string") {    
            ASTFromInputSemanticTC("var titi = 2; var toto = \"test\"; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("int x float") {
            ASTFromInputSemanticTC("var titi = 2; var toto = 5.2; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("float x int") {
            ASTFromInputSemanticTC("var titi = 2.0; var toto = 5; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("string x int") {    
            ASTFromInputSemanticTC("var titi = \"tititititit\"; var toto = 6; titi * toto;", parser_test, table_test, type_test);
        }

        SUBCASE("string = int") {    
            ASTFromInputSemanticTC("var titi = \"strrrr\"; titi = 3;", parser_test, table_test, type_test);  
        }

        SUBCASE("float = string") {
            ASTFromInputSemanticTC("var titi = 0.1; titi = \"toto\";", parser_test, table_test, type_test);
        }

        SUBCASE("float = string (variable)") {
            ASTFromInputSemanticTC("var titi = 0.1; var toto = \"toto\"; titi = toto;", parser_test, table_test, type_test);
        }
        
        SUBCASE("float = float") {
            ASTFromInputSemanticTC("var titi = 0.1; titi = 0.2;", parser_test, table_test, type_test);
        }

        SUBCASE("float = int") {
            ASTFromInputSemanticTC("var titi = 0.1; titi = 2;", parser_test, table_test, type_test);
        }

        SUBCASE("int = float") {
            ASTFromInputSemanticTC("var titi = 1; titi = 0.2;", parser_test, table_test, type_test);
        }
        
        SUBCASE("int = string") {
            ASTFromInputSemanticTC("var titi = 7; titi = \"123\";", parser_test, table_test, type_test);
        }
        
        SUBCASE("float = string") {
            ASTFromInputSemanticTC("var titi = 7.8; titi = \"123\";", parser_test, table_test, type_test);
        }
        
        SUBCASE("function call : string") {
            ASTFromInputSemanticTC("var titi = function(test:string) {}; titi(\"lol\");", parser_test, table_test, type_test);
        }

        SUBCASE("Calling a function with a type convertible argument") {
            ASTFromInputSemanticTC("var titi = function(test:string) {}; titi(23);", parser_test, table_test, type_test);
        }

        SUBCASE("constructor with 1 parameter") {
            ASTFromInputSemanticTC("var Joueur = function(nomL:string) : var { return { nom : nomL }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.nom;", parser_test, table_test, type_test);
        }

		SUBCASE("constructor with 1 parameter same name") {
			ASTFromInputSemanticTC("var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.nom;", parser_test, table_test, type_test);
		}

		SUBCASE("constructor complex with contained function NOT USING the current type...") {
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
			"joueur1.attaquer(joueur2);"
				, parser_test, table_test, type_test);
		}

		/*SUBCASE("constructor complex with contained function USING the current type...") {
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
                ASTFromInputSemanticTC("var i = 120; i = function() {};", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

        SUBCASE("Because of non-matching type (function then variable)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = 9;", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }
        
        SUBCASE("Reassigning function (function = function)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = function(ttt:string) {};", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

        SUBCASE("Calling a function with wrong type arguments") {
            try {
                ASTFromInputSemanticTC("var titi = function(test:function) {}; titi(23);", parser_test, table_test, type_test);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(true);
            }
        }

		SUBCASE("constructor with 1 parameter + bad field access") {
			try {
				ASTFromInputSemanticTC("var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.ttetetetet;", parser_test, table_test, type_test);
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
				, parser_test, table_test, type_test);
				CHECK(false);
			} catch (std::exception& e) {
				//std::cout << e.what() << std::endl;
				CHECK(true);
			}
		}
    }
}
