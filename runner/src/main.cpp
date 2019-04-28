#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Interpreter/Interpreter.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

int main(int argc, char* argv[]) {
	if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
    }
	
    const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCache{};
	auto typeCrosser = ska::TypeCrosser{};
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser, typeCrosser };
	auto symbolsTypeUpdater = ska::SymbolTableTypeUpdater {parser};
	auto typeChecker = ska::SemanticTypeChecker {parser, typeCrosser };
	auto interpreter = ska::Interpreter {reservedKeywords, typeCrosser };

	auto inputFile = std::ifstream{ std::string{argv[1]} + ".miniska" };
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << ".miniska\"";
        return -1;
	}
	
	auto scriptEmBinding = ska::ScriptBridge{ scriptCache, "em_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
	
	auto memInputComponent = scriptEmBinding.createMemory();
	scriptEmBinding.bindFunction("setInputMovePower", std::function<void(int, int)>([](int characterId, int value) {
		std::cout << "move power for " << characterId << " is now " << value << std::endl;
	}));
	scriptEmBinding.bindFunction("setInputJumpPower", std::function<void(int, int)>([](int characterId, int value) {
		std::cout << "jump power for " << characterId << " is now " << value << std::endl;
	}));
	scriptEmBinding.build();

	auto scriptCharacterBinding = ska::ScriptBridge{ scriptCache, "character_generator", typeBuilder, symbolsTypeUpdater, reservedKeywords };
	scriptCharacterBinding.import(parser, interpreter, { {"Character", "character"} });
	scriptCharacterBinding.bindGenericFunction("Gen", { "Character::Fcty" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
		auto parametersValues = std::vector<ska::NodeValue>{};
		parametersValues.push_back("toto");
		return scriptCharacterBinding.callFunction(interpreter, "Character", "Fcty", std::move(parametersValues));
	}));
	scriptCharacterBinding.build();

	auto scriptBinding = ska::ScriptBridge{ scriptCache, "runner_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
	scriptBinding.bindFunction("printInt", std::function<void(int)>([](int value) {
		std::cout << value << std::endl;
	}));
	
	scriptBinding.bindFunction("printString", std::function<void(std::string)>([](std::string value) {
		std::cout << value << std::endl;
	}));
	scriptBinding.build();

	auto executor = ska::Script{ scriptCache, "main", ska::Tokenizer{ reservedKeywords, 
	"var Player = import \"" + std::string{argv[1]} + "\"; var CharacterGenerator = import \"character_generator\"; Player.run(CharacterGenerator.Gen());"
	}.tokenize() };

	try {
		executor.parse(parser);
		interpreter.script(executor);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}


