#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Interpreter/Value/Script.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

auto readerSTC = std::unique_ptr<ska::Script>{};

static ska::Script ASTFromInputSemanticTC(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, DataTestContainer& data) {

	auto tokenizer = ska::Tokenizer { data.reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto typeCrosser = ska::TypeCrosser{};
	readerSTC = std::make_unique<ska::Script>(scriptCache, "main", tokens );
	data.parser = std::make_unique<ska::StatementParser> ( data.reservedKeywords );
    data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosser);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosser);
    readerSTC->parse(*data.parser);
    return *readerSTC;
}

TEST_CASE("[SymbolTableUpdater] update node symbol") {
	DataTestContainer data;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	auto astPtr = ASTFromInputSemanticTC(scriptCache, "var i = 0; var titi = \"llllll\"; { var toto = 2; var i = titi; }", data);

	auto& table = astPtr.symbols();

	CHECK(table.nested().size() == 1);
	const auto* i = table["i"];
	const auto* nestedToto = (*table.nested()[0])["toto"];
	const auto* toto = table["toto"];
	const auto* titi = table["titi"];
	const auto* nestedI = (*table.nested()[0])["i"];

	auto& ast = astPtr.rootNode();
	CHECK(i == ast[0].symbol());
	CHECK(titi == ast[1].symbol());
	CHECK(ast[1].symbol() != ast[0].symbol());
	CHECK(nestedToto == ast[2][0].symbol());
	CHECK(nestedI == ast[2][1].symbol());
	CHECK(nestedI != i);
}

TEST_CASE("[SymbolTableUpdater] function declaration") {
	DataTestContainer data;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = function() {};", data);

	auto& table = astPtr.symbols();

	const auto* toto = table["toto"];

	auto& ast = astPtr.rootNode();
	CHECK(toto != nullptr);
	CHECK(toto == ast[0].symbol());
	CHECK(toto == ast[0][0].symbol());
}

TEST_CASE("[SemanticTypeChecker]") {
    DataTestContainer data;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	SUBCASE("OK") {
        SUBCASE("boolean") {
            auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 3 == 3;", data);
            auto& ast = astPtr.rootNode();
            CHECK(ast.size() == 1);
            CHECK(ast[0].type() == ska::ExpressionType::BOOLEAN);
        }

		SUBCASE("Cross type") {
			SUBCASE("float x float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 2.0; var toto = 5.2; titi * toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("int x string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 2; var toto = \"test\"; titi * toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("int x float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 2; var toto = 5.2; titi * toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("float x int") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 2.0; var toto = 5; titi * toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("string x int") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = \"tititititit\"; var toto = 6; titi * toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("string = int") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = \"strrrr\"; titi = 3;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("float = string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 0.1; titi = \"toto\";", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("float = string (variable)") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 0.1; var toto = \"toto\"; titi = toto;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("float = float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 0.1; titi = 0.2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("float = int") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 0.1; titi = 2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("int = float") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 1; titi = 0.2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::INT);
			}

			SUBCASE("int = string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 7; titi = \"123\";", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::INT);
			}

			SUBCASE("float = string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi = 7.8; titi = \"123\";", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::FLOAT);
			}

			SUBCASE("not an lvalue") {
				try {
					ASTFromInputSemanticTC(scriptCache, "var testIf188 = 3; var lvalFunc = function() : int { return 0; }; lvalFunc() = testIf188;", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("The symbol \"\" is not an lvalue, therefore cannot be assigned") != std::string::npos);
				}
			}

		}

		SUBCASE("Parameters") {
			SUBCASE("function call : string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi121 = function(test:string) {}; titi121(\"lol\");", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::VOID);
			}


			SUBCASE("Calling a function with a type convertible argument") {
				ASTFromInputSemanticTC(scriptCache, "var titi129 = function(test:string) {}; titi129(23);", data);
			}

			SUBCASE("constructor with 1 parameter") {
				ASTFromInputSemanticTC(scriptCache, "var Joueur1 = function(nomL:string) : var { return { nom : nomL }; }; var joueur1 = Joueur1(\"joueur 1\"); joueur1.nom;", data);
			}

			SUBCASE("constructor with 1 parameter same name") {
				ASTFromInputSemanticTC(scriptCache, "var Joueur2 = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur2(\"joueur 1\"); joueur1.nom;", data);
			}
		}

        SUBCASE("return") {
            SUBCASE("built-in") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var testReturn148 = function() : int { return 2543; }; var value = testReturn148(); value;", data);
                auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::ExpressionType::INT);
            }

            SUBCASE("bad built-in type (no return type mentioned = void)") {
                try {
                    ASTFromInputSemanticTC(scriptCache, "var testReturn148 = function() { return 2543; }; var value = testReturn148(); value;", data);
                    CHECK(false);
                } catch (std::exception& e) {
                    CHECK(std::string{e.what()}.find("bad return type : expected \"void\" on function declaration but got \"int\" on return") != std::string::npos);
                }
            }

			SUBCASE("bad custom and built-in type mix") {
				try {
					ASTFromInputSemanticTC(scriptCache, "var testReturn148 = function() { return { toto : 2 }; }; var value = testReturn148(); value;", data);
					CHECK(false);
				} catch (std::exception & e) {
					CHECK(std::string{e.what()}.find("bad return type : expected \"void\" on function declaration but got \"var\" on return") != std::string::npos);
				}
			}

			SUBCASE("bad return : not placed in direct function-constructor") {
				try {
					ASTFromInputSemanticTC(scriptCache, "var testReturn148 = function() : int { if(true) { return 2543; } };", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("bad user-defined return placing : custom return must be set in a named function-constructor") != std::string::npos);
				}
			}

			SUBCASE("code after return : no effect") {
				ASTFromInputSemanticTC(scriptCache, "var testReturn148 = function() : int { return 2543; var test = 2; };", data);
			}
        }

		SUBCASE("Conditions") {
			SUBCASE("bad type : not a bool") {
				try {
					ASTFromInputSemanticTC(scriptCache, "var testIf188 = 3; if(testIf188) {}", data);
					CHECK(false);
				} catch (std::exception& e) {
					CHECK(std::string{e.what()}.find("expression condition is not a boolean (it's a \"int\")") != std::string::npos);
				}
			}

			SUBCASE("type OK : bool") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var testIf188 = true; if(testIf188) {}", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[0][0].type() == ska::ExpressionType::BOOLEAN);
				CHECK(ast[1][0].type() == ska::ExpressionType::BOOLEAN);
			}

			SUBCASE("type OK : bool indirect") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var testIf188 = 3 == 3; if(testIf188) {}", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::BOOLEAN);
			}

		}

		SUBCASE("Loop") {
				SUBCASE("using inner for variable successively ") {
            ASTFromInputSemanticTC(scriptCache, "for(var i = 0; i < 10; i = i + 1); for(var i = 0; i < 10; i = i + 1);", data);
        }
		}

		SUBCASE("Array") {

			SUBCASE("string") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str152 = [\"tt\", \"titi\"];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 1);
				CHECK(ast[0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[0].type().value().size() == 1);
				CHECK(ast[0].type().value().compound()[0] == ska::ExpressionType::STRING);
			}

			SUBCASE("string : cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str159 = [\"tt\", \"titi\"]; str159[0];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::ExpressionType::STRING);
			}

			SUBCASE("int : add a cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25]; toto = toto + 4;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25, 3]; toto = toto - 1;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell range") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25, 3]; var range = [0, 1]; toto = toto - range;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("int : del a cell range, in-place value") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25, 3]; toto = toto - [0, 1];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
			}

			SUBCASE("double array string : cell") {

                auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str167 = [[0, 1], [2, 3]]; str167[0]; str167[0][0];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[1].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[2].type() == ska::ExpressionType::INT);

            }

			SUBCASE("function call : array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var titi131 = function(test131:string[]) {}; var strArray131 = [\"lol\", \"toto\"]; titi131(strArray131);", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
			}

			SUBCASE("expression-array") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var186 = function() : var { var toto = [0]; return { toto : toto }; }; var186().toto[0];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::ARRAY);
				CHECK(ast[1][0].type().value().compound()[0] == ska::ExpressionType::INT);
			}

            SUBCASE("expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var193 = function() : var { var toto = [0]; return { toto : toto }; }; 2 * 3 + var193().toto[0];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var200 = function() : var { var toto = [0]; return { toto : toto }; }; 2 + 3 * var200().toto[0];", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("post expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var201 = function() : var { var toto = [0]; return { toto : toto }; }; var201().toto[0] * 3 + 2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("post expression-array with use in expression 2") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var202 = function() : var { var toto = [0]; return { toto : toto }; }; var202().toto[0] + 3 * 2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
                CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("complex expression-array with use in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var203 = function() : var { var toto = [0]; return { toto : toto }; }; (5 + var203().toto[0] + 3 * 4) * 2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 2);
				CHECK(ast[1][0].type() == ska::ExpressionType::INT);
			}

            SUBCASE("complex expression-array with variable used in expression") {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var203 = function() : var { var toto = [0]; return { toto : toto }; }; var totoVar = var203().toto; (5 + totoVar[0] + 3 * 4) * 2;", data);
				auto& ast = astPtr.rootNode();
				CHECK(ast.size() == 3);
				CHECK(ast[2][0].type() == ska::ExpressionType::INT);
			}

			SUBCASE("Fail") {
				SUBCASE("not an array") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "var var187 = 123; var187[0];", data);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(std::string{e.what()}.find("expression is not an array (it's a \"int\")") != std::string::npos);
					}
				}

                 SUBCASE("operator in wrong order : ") {
                    try {
                        auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str242 = [0, 1]; str242*[0] 2;", data);
                        auto& ast = astPtr.rootNode();
                        CHECK(ast.size() == 2);
                        CHECK(ast[1].type() == ska::ExpressionType::INT);
                    } catch(std::exception& e) {
                        CHECK(std::string{e.what()}.find("Unable to use operator \"*\" on types \"array\" and \"int\"") != std::string::npos);
                    }
                }

                SUBCASE("syntax error ']'") {
                     try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str170 = [\"tt\", \"titi\"]];", data);
						CHECK(false);
					} catch (std::exception& e) {
						CHECK(true);
					}
				}

				SUBCASE("int : bad operation : operator - with string") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25, 3]; toto = toto - \"1\";", data);
						auto& ast = astPtr.rootNode();
						CHECK(ast.size() == 2);
						CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
					} catch(std::exception& e) {
						CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"array\" and \"string\"") != std::string::npos);
					}
				}

				SUBCASE("int : bad operation : operator - with int, reverse ordered") {
					try {
						auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [14, 25, 3]; toto = 1 - toto;", data);
						auto& ast = astPtr.rootNode();
						CHECK(ast.size() == 2);
						CHECK(ast[1].type() == ska::Type::MakeBuiltInArray(ska::ExpressionType::INT));
					} catch(std::exception& e) {
						CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"int\" and \"array\"") != std::string::npos);
					}
				}
			}
		}
	}

    SUBCASE("Fail") {

		SUBCASE("Because of undeclared symbol in expression") {
			try {
				ASTFromInputSemanticTC(scriptCache, "tti;", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("Symbol not found : tti") != std::string::npos);
			}
		}

        SUBCASE("Because of non-matching type (variable then function)") {
            try {
                ASTFromInputSemanticTC(scriptCache, "var i = 120; i = function() {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string{e.what()}.find("Symbol already exists : i") != std::string::npos);
            }
        }

        SUBCASE("Because of non-matching type (function then variable)") {
            try {
                ASTFromInputSemanticTC(scriptCache, "var titi = function() {}; titi = 9;", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string{e.what()}.find("Unable to use operator \"=\" on types \"function\" and \"int\"") != std::string::npos);
            }
        }

        SUBCASE("Reassigning function (function = function)") {
            try {
                ASTFromInputSemanticTC(scriptCache, "var titi = function() {}; titi = function(ttt:string) {};", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string{e.what()}.find("Symbol already exists : titi") != std::string::npos);
            }
        }

        SUBCASE("Calling a function with wrong type arguments") {
            try {
                ASTFromInputSemanticTC(scriptCache, "var titi = function(test:function) {}; titi(23);", data);
                CHECK(false);
            } catch(std::exception& e) {
                CHECK(std::string{e.what()}.find("Unable to use operator \"=\" on types \"function\" and \"int\"") != std::string::npos);
            }
        }

		SUBCASE("constructor with 1 parameter + bad field access") {
			try {
				ASTFromInputSemanticTC(scriptCache, "var Joueur = function(nom:string) : var { return { nom : nom }; }; var joueur1 = Joueur(\"joueur 1\"); joueur1.ttetetetet;", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("trying to access to an undeclared field : \"ttetetetet\" of \"joueur1\"") != std::string::npos);
			}
		}

		SUBCASE("Array : heterogen types") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var toto = [1, 2.0];", data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{e.what()}.find("array has not uniform types in it : \"int\" and \"float\"" ) != std::string::npos);
			}
		}

		//TODO : move this test in TypeCross_test
		SUBCASE("Array : del a range of cell : failure bad type (double)") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache,"var toto = [14.0, 25.0, 13.0, 2.0]; toto = toto - [1.0, 2.0];", data);
				CHECK(false);
			} catch (std::exception & e) {
				CHECK(std::string{e.what()}.find("Unable to use operator \"-\" on types \"array (float)\" and \"array (float)\"" ) != std::string::npos);
			}
		}

		SUBCASE("double array string : cell") {
			try {
				auto astPtr = ASTFromInputSemanticTC(scriptCache, "var str167 = [[0, 1], [2.0]]; str167[1]; str167[1][0];", data);
				CHECK(false);
			} catch (std::exception& e) {
				CHECK(std::string{e.what()}.find("array has not uniform types in it : \"array (int)\" and \"array (float)\"" ) != std::string::npos);
			}
		}

		SUBCASE("Method call in if condition statement [direct]") {
			auto astPtr = ASTFromInputSemanticTC(scriptCache,
			"var Fcty230 = function() : var { return { size : function(): int { return 0;}}; };\n"
			"if (Fcty230().size() > 0) {}", data);
		}

		SUBCASE("constructor complex with contained function NOT USING the current type and calling member function with a wrong type...") {
			try {
				ASTFromInputSemanticTC(scriptCache,
				"var Joueur = function(nom:string) : var { "
					"var puissance = 10;"

					"var attaquer = function(cible:Joueur()) {"
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
				CHECK(std::string{e.what()}.find("Unable to use operator \"=\" on types \"var\" and \"int\"") != std::string::npos);
			}
		}
    }
}
