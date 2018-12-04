#include <iostream>

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
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser);
    return data.parser->parse();
}

TEST_CASE("[SemanticTypeChecker]") {
    DataTestContainer data;
    
	SUBCASE("OK") {
		SUBCASE("Cross type") {
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
		}

		SUBCASE("Parameters") {
			SUBCASE("function call : string") {
				auto astPtr = ASTFromInputSemanticTC("var titi121 = function(test:string) {}; titi121(\"lol\");", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
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
		}

		SUBCASE("Array") {
			//TODO
			SUBCASE("empty") {
				//auto astPtr = ASTFromInputSemanticTC("var str146 = [];", data);
				//auto& ast = (*astPtr);
			}

			SUBCASE("string") {
				auto astPtr = ASTFromInputSemanticTC("var str152 = [\"tt\", \"titi\"];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 1);
				CHECK(ast[0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[0].type().value().size() == 1);
				CHECK(ast[0].type().value().compound()[0] == ska::ExpressionType::STRING);
			}

			SUBCASE("string : cell") {
				auto astPtr = ASTFromInputSemanticTC("var str159 = [\"tt\", \"titi\"]; str159[0];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::STRING);
			}

			//TODO
			SUBCASE("double array string : cell") {
				/*
                auto astPtr = ASTFromInputSemanticTC("var str167 = [[0, 1], [2, 3]]; str167[0]; str167[0][0];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 3);
				CHECK(ast[1].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[2].type() == ska::ExpressionType::INT);
			    */
            }

			SUBCASE("function call : array") {
				auto astPtr = ASTFromInputSemanticTC("var titi131 = function(test131:string[]) {}; var strArray131 = [\"lol\", \"toto\"]; titi131(strArray131);", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 3);
			}

			SUBCASE("expression-array") {
				auto astPtr = ASTFromInputSemanticTC("var var186 = function() : var { var toto = [0]; return { toto : toto }; }; var186().toto[0];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[1][0].type().value().compound()[0] == ska::ExpressionType::INT);
			}

            SUBCASE("expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC("var var193 = function() : var { var toto = [0]; return { toto : toto }; }; 2 * 3 + var193().toto[0];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC("var var200 = function() : var { var toto = [0]; return { toto : toto }; }; 2 + 3 * var200().toto[0];", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("post expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC("var var201 = function() : var { var toto = [0]; return { toto : toto }; }; var201().toto[0] * 3 + 2;", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}
            
            SUBCASE("post expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC("var var202 = function() : var { var toto = [0]; return { toto : toto }; }; var202().toto[0] + 3 * 2;", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
                std::cout << ast[1][0].type().value() << std::endl;
                CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("complex expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC("var var203 = function() : var { var toto = [0]; return { toto : toto }; }; (5 + var203().toto[0] + 3 * 4) * 2;", data);
				auto& ast = (*astPtr);
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("Fail") {
				SUBCASE("not an array") {
					try {
						auto astPtr = ASTFromInputSemanticTC("var var187 = 123; var187[0];", data);
						auto& ast = (*astPtr);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(std::string("expression is not an array (it's a  int)") == e.what());
					}
				}
                
                //TODO
                SUBCASE("syntax error ']'") {
					/*
                     try {
						auto astPtr = ASTFromInputSemanticTC("var str170 = [\"tt\", \"titi\"]];", data);
						auto& ast = (*astPtr);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(true);
					}
                    */
				}
			}
		}
	}

    SUBCASE("Fail") {
        SUBCASE("Because of non-matching type (variable then function)") {
            try {
                ASTFromInputSemanticTC("var i = 120; i = function() {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string(e.what()) == "Symbol already exists : i");
            }
        }

        SUBCASE("Because of non-matching type (function then variable)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = 9;", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string(e.what()) == "Unable to use operator \"=\" on types function and function");
            }
        }
        
        SUBCASE("Reassigning function (function = function)") {
            try {
                ASTFromInputSemanticTC("var titi = function() {}; titi = function(ttt:string) {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string(e.what()) == "Symbol already exists : titi");
            }
        }

        SUBCASE("Calling a function with wrong type arguments") {
            try {
                ASTFromInputSemanticTC("var titi = function(test:function) {}; titi(23);", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string(e.what()) == "Unable to use operator \"=\" on types function and int");
            }
        }

		SUBCASE("constructor with 1 parameter + bad field access") {
			try {
				ASTFromInputSemanticTC("var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.ttetetetet;", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string(e.what()) == "trying to access to an undeclared field : ttetetetet of joueur1 of type Joueur (function -  string - Joueur var)");
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
				CHECK(std::string(e.what()) == "Unable to use operator \"=\" on types var and int");
			}
		}
    }
}
