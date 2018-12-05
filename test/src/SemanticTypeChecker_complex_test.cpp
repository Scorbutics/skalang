#include <doctest.h>
#include "LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "SymbolTable.h"
#include "ReservedKeywordsPool.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "TypeBuilder.h"

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticComplexTC(const std::string& input, DataTestContainer& data) {
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

TEST_CASE("[SemanticTypeChecker Complex]") {
	DataTestContainer data;
	SUBCASE("constructor with function field accessed through variable") {
		ASTFromInputSemanticComplexTC(
			"var JoueurClass = function(nom:string) : var { "
				"var attaquer = function(degats:int) {"
				"};"

				"return {"
					"attaquer : attaquer"
				"};"
			"};"
			"var joueur1 = JoueurClass(\"joueur1Nom\");"
			"joueur1.attaquer(10);"
			, data);
	}

	SUBCASE("class with internal data used in public function") {
		ASTFromInputSemanticComplexTC(
			"var Stats = function() : var {"
				"var pointsDeVie = 100;"
				"var blesser = function(degats:int) {"
					"pointsDeVie = pointsDeVie - degats;"
				"};"
				"return {"
					"blesser : blesser"
				"};"
			"};"
			"var JoueurClass = function(nom:string) : var { "
				"var stats = Stats();"

				"var attaquer = function(degats:int) {"
					"stats.blesser(degats);"
				"};"

				"return {"
					"attaquer : attaquer,"
					"stats : stats"
				"};"
			"};"
			"var joueur1 = JoueurClass(\"joueur1Nom\");"
			"joueur1.attaquer(1);"
			"joueur1.stats.blesser(1);"
			, data);
	}

    SUBCASE("constructor with integer field accessed through function call and used in expression") {
		ASTFromInputSemanticComplexTC(
			"var JoueurClass = function(nom:string) : var { "
				"var test74 = 123;"

				"return {"
					"test : test74"
				"};"
			"};"
            "(5 + JoueurClass(\"joueur1Nom\").test + 3 * 4) * 2;"
			, data);
	}

	SUBCASE("class with internal data used in public function 2") {
		ASTFromInputSemanticComplexTC(
			"var Stats = function() : var {"
				"var pointsDeVie = 100;"
				"var blesser = function(degats:int) {"
					"pointsDeVie = pointsDeVie - degats;"
				"};"
				"return {"
					"blesser : blesser,"
					"pdv : pointsDeVie"
				"};"
			"};"
			"var JoueurClass = function(nom:string) : var { "
				"var stats = Stats();"

				"var attaquer = function(statsDegats:Stats) {"
					"stats.blesser(statsDegats.pdv);"
				"};"

				"return {"
					"attaquer : attaquer"
				"};"
			"};"
			"var joueur1 = JoueurClass(\"joueur1Nom\");"
			"var statsDeg = Stats();"
			"joueur1.attaquer(statsDeg);"
			, data);
	}


	SUBCASE("constructor complex with contained function NOT USING the current type BUT mentioning it...") {
		ASTFromInputSemanticComplexTC(
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
		"var joueur1 = JoueurClass(\"joueur1Nom\");"
		"var joueur2 = JoueurClass(\"joueur2Nom\");"
		"joueur1.attaquer(joueur2);"
			, data);
	}



	//TODO : complexe à réaliser mais serait bien.
	SUBCASE("constructor complex with contained function USING the current type...") {
		try {
			ASTFromInputSemanticComplexTC(
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
				, data);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string(e.what()) == 
				"the class symbol table pv is not registered. Maybe you're trying to use the type you're defining in its definition...");
		}
	}
}