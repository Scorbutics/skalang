#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "DataTestContainer.h"
#include "Service/SymbolTable.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/Script.h"

ska::Script ASTFromInputSemanticComplexTC(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script {scriptCache, "main", tokens };
	
    data.parser = std::make_unique<ska::StatementParser> ( reservedKeywords );
    data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser);
    reader.parse(*data.parser);
    return reader;
}

TEST_CASE("[SemanticTypeChecker Complex]") {
	DataTestContainer data;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};

	SUBCASE("constructor with function field accessed through variable") {
		ASTFromInputSemanticComplexTC(scriptCache,
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


    SUBCASE("constructor with integer field accessed through function call and used in expression") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"var JoueurClass = function(nom:string) : var { "
				"var test74 = 123;"

				"return {"
					"test : test74"
				"};"
			"};"
            "(5 + JoueurClass(\"joueur1Nom\").test + 3 * 4) * 2;"
			, data);
	}

	SUBCASE("constructor complex with contained function NOT USING the current type BUT mentioning it...") {
		ASTFromInputSemanticComplexTC(scriptCache,
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

	SUBCASE("class with internal data used in public function") {
		ASTFromInputSemanticComplexTC(scriptCache,
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

	SUBCASE("class with internal data used in public function 2") {
		ASTFromInputSemanticComplexTC(scriptCache,
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

			"var attaquer = function(statsDegats:Stats, test:int) {"
			"stats.blesser(statsDegats.pdv);"
			"};"

			"return {"
			"attaquer : attaquer"
			"};"
			"};"
			"var joueur1 = JoueurClass(\"joueur1Nom\");"
			"var statsDeg = Stats();"
			"joueur1.attaquer(statsDeg, 0);"
			, data);
	}

	SUBCASE("after field access, not an lvalue") {
		try {
			ASTFromInputSemanticComplexTC(scriptCache,
				"var lvalFunc137 = function() : var {" 
					"var test137_ = function() : int { return 0; };" 
					"return { test : test137_};" 
				"};" 
				"var object = lvalFunc137();"
				"object.test() = 1234;", data);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(e.what() == std::string("The symbol \"\" is not an lvalue, therefore cannot be assigned"));
		}
	}
	
	SUBCASE("function with 2 return placements (early return support)") {
		ASTFromInputSemanticComplexTC(scriptCache, "var f_semantic151 = function(titi:int) : int { if(titi == 0) { return 1; } return 0; }; var int_semantic151 = f_semantic151(1);", data);
	}

}
