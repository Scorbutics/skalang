#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Config/LoggerConfigLang.h"
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

#include "std/module/io.h"

int main(int argc, char* argv[]) {
	if (argc <= 1) {
        std::cout << "No file name entered. Exiting..." << std::endl;
        return -1;
    }
	
	auto inputFile = std::ifstream{ std::string{argv[1]} + ".miniska" };
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << ".miniska\"" << std::endl;
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

	auto moduleConfiguration = ska::lang::ModuleConfiguration{scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords};

	try {
		auto scriptEmBinding = ska::ScriptBridge{ scriptCache, "em_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptEmBinding.bindFunction("setInputMovePower", std::function<void(int, int)>([](int characterId, int value) {
			std::cout << "move power for " << characterId << " is now " << value << std::endl;
		}));
		scriptEmBinding.bindFunction("setInputJumpPower", std::function<void(int, int)>([](int characterId, int value) {
			std::cout << "jump power for " << characterId << " is now " << value << std::endl;
		}));
		scriptEmBinding.bindFunction("removeInputComponent", std::function<void(int)>([](int characterId) { }));
		scriptEmBinding.bindFunction("restoreInputComponent", std::function<void(int, int, int)>([](int characterId, int movePower, int jumpPower) {
			std::cout << "restoring input with move " << movePower << " and jump " << jumpPower << std::endl;
		}));
		scriptEmBinding.buildFunctions();

		auto scriptParametersBinding = ska::ScriptBridge{ scriptCache, "parameters_gen_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptParametersBinding.import(parser, interpreter, { {"Parameters", "parameters"} });
		scriptParametersBinding.bindGenericFunction("Gen", { "Parameters::Fcty" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto result = scriptParametersBinding.createMemory();
			result->emplace("asInt", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
				auto index = params[0].nodeval<int>();
				std::cout << "accessing parameter " << index << " as int" << std::endl;
				return ska::NodeValue{ index };
			})));
			result->emplace("asString", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
				std::cout << "accessing parameter " << params[0].nodeval<int>() << " as string" << std::endl;
				return ska::NodeValue{ "toto" };
			})));
			return result;
		}));
		scriptParametersBinding.bindFunction("asString", std::function<std::string(int)>([](int index) {
			return "";
		}));
		scriptParametersBinding.buildFunctions();

		auto scriptCharacterCommands = ska::ScriptBridge{ scriptCache, "character_commands_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptCharacterCommands.bindFunction("jump", std::function<void(int)>([](int index) {
			std::cout << "jump ! " << index << std::endl;
		}));
		scriptCharacterCommands.bindFunction("move", std::function<void(int, int, int)>([](int direction, int power, int t) {
			std::cout << "move ! direction " << direction << " power " << power << " ??? " << t << std::endl;
		}));
		scriptCharacterCommands.buildFunctions();

		auto scriptCharacterBinding = ska::ScriptBridge{ scriptCache, "character_generator", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptCharacterBinding.import(parser, interpreter, { {"Character", "character"} });
		scriptCharacterBinding.bindGenericFunction("Gen", { "Character::Fcty" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto parametersValues = std::vector<ska::NodeValue>{};
			parametersValues.push_back("toto");
			return scriptCharacterBinding.callFunction(interpreter, "Character", "Fcty", std::move(parametersValues));
		}));
		scriptCharacterBinding.buildFunctions();

		ska::lang::UseModuleIO(moduleConfiguration);

		auto executor = ska::Script{ scriptCache, "main", ska::Tokenizer{ reservedKeywords, 
		"var Script = import \"" + std::string{argv[1]} + "\"; var CharacterGenerator = import \"character_generator\"; var ParametersGenerator = import \"parameters_gen_lib\"; Script.run(CharacterGenerator.Gen(), ParametersGenerator.Gen());"
		}.tokenize() };

	
		executor.parse(parser);
		interpreter.script(executor);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}


