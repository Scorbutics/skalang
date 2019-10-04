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
#include "NodeValue/ScriptAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

ska::ScriptAST ASTFromInputSemanticComplexTC(ska::ScriptCacheAST& scriptCache, const std::string& input, DataTestContainer& data) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::ScriptAST {scriptCache, "main", tokens };
	auto typeCrosser = ska::TypeCrosser{};

    data.parser = std::make_unique<ska::StatementParser> ( reservedKeywords );
    data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosser);
	data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	data.typeChecker = std::make_unique<ska::SemanticTypeChecker>(*data.parser, typeCrosser);
    reader.parse(*data.parser);
    return reader;
}

TEST_CASE("[SemanticTypeChecker Complex]") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};

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

			"var attaquer = function(cible:JoueurClass()) {"
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

			"var attaquer = function(statsDegats:Stats(), test:int) {"
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
			CHECK(std::string{e.what()}.find("The symbol \"\" is not an lvalue, therefore cannot be assigned") != std::string::npos);
		}
	}

	SUBCASE("assign 2 differents object types") {
		try {
			ASTFromInputSemanticComplexTC(scriptCache,
				"var lvalFunc160 = function() : var {"
				"return {};"
				"};"
				"var lvalFunc163 = function() : var {"
				"return {};"
				"};"
				"var object = lvalFunc160();"
				"object = lvalFunc163();", data);
			CHECK(false);
		} catch (std::exception & e) {
			CHECK(std::string{e.what()}.find("The symbol \"object\" has already been declared as \"var lvalFunc160\" but is now wanted to be \"var lvalFunc163\"") != std::string::npos);
		}
	}

	SUBCASE("Function 0 parameter creating custom object but forget to use it as a factory (direct use of function)") {
		try {
			auto astPtr = ASTFromInputSemanticComplexTC(scriptCache, "var Dummy = function() : var { return { data: 3 }; }; Dummy.data; ", data);
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("the variable \"Dummy\" is not registered as an object but as a \"function Dummy (var Dummy)\"") != std::string::npos);
		}
	}

	SUBCASE("return a concrete custom type") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"var lvalFunc185 = function() : var {"
			"return { test : 14 };"
			"};"
			"var lvalFunc188 = function() : lvalFunc185() {"
			"return lvalFunc185();"
			"};"
			"var object = lvalFunc188();"
			"object.test = 1234;", data);
	}

	SUBCASE("return a concrete custom type with namespace") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"var Character = import \"" SKALANG_TEST_DIR "/src/resources/character\";"
			"var lvalFunc188 = function() : Character::build() {"
				"return Character.build(\"t\");"
			"};"
			"var object = lvalFunc188();"
			"object.age = 1234;", data);
	}

	SUBCASE("using a function as a parameter") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"var lvalFunc206 = function() : var {"
				"return { test : 14 };"
			"};"
			"var lvalFunc209 = function(toto: lvalFunc206) : lvalFunc206() {"
				"return toto();"
			"};"
			"var object = lvalFunc209(lvalFunc206);"
			"object.test = 1234;", data);
	}

	SUBCASE("using a callback function as a parameter without using the source type (function type compatibility)") {
		ASTFromInputSemanticComplexTC(scriptCache,
			"var lvalFunc218 = function() {};"
			"var lvalFunc219 = function(toto: lvalFunc218) : lvalFunc218() {"
			"	toto();"
			"};"
			"var callback = function() {};"
			"lvalFunc219(callback);", data);
	}

	SUBCASE("for with empty statement in if") {
		ASTFromInputSemanticComplexTC(scriptCache, 
			"var size = function() : int { return 10;};"
			"if (size() > 0) {"
				"for(var i = 0; i < size(); i = i + 1);"
			"}", data);
	}
}
