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
#include "Service/ScriptNameBuilder.h"
#include "Interpreter/Interpreter.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

#include "std/module/io.h"
#include "std/module/parameter.h"

int main(int argc, char* argv[]) {
	if (argc <= 1) {
        std::cout << "No file name entered. Exiting..." << std::endl;
        return -1;
    }
	
	auto inputFile = std::ifstream{ std::string{argv[1]}};
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << std::endl;
        return -1;
	}

	//TODO configuration
#ifdef _MSC_VER 
	ska::ScriptNameStandardLibraryPrefix() = "../../runner/scripts/std";
#else
	ska::ScriptNameStandardLibraryPrefix() = "../runner/scripts/std";
#endif

    const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCache{};
	auto typeCrosser = ska::TypeCrosser{};
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser, typeCrosser };
	auto symbolsTypeUpdater = ska::SymbolTableTypeUpdater {parser};
	auto typeChecker = ska::SemanticTypeChecker {parser, typeCrosser };
	auto interpreter = ska::Interpreter {reservedKeywords, typeCrosser };

	auto moduleConfiguration = ska::lang::ModuleConfiguration {scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords, parser, interpreter};

	try {
		auto scriptEmBinding = ska::ScriptBridge{ scriptCache, "em_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptEmBinding.bindFunction("setInputMovePower", std::function<void(ska::Script&, int, int)>([](ska::Script&, int characterId, int value) {
			std::cout << "move power for " << characterId << " is now " << value << std::endl;
		}));
		scriptEmBinding.bindFunction("setInputJumpPower", std::function<void(ska::Script&, int, int)>([](ska::Script&, int characterId, int value) {
			std::cout << "jump power for " << characterId << " is now " << value << std::endl;
		}));
		scriptEmBinding.bindFunction("removeInputComponent", std::function<void(ska::Script&, int)>([](ska::Script&, int characterId) { }));
		scriptEmBinding.bindFunction("restoreInputComponent", std::function<void(ska::Script&, int, int, int)>([](ska::Script&, int characterId, int movePower, int jumpPower) {
			std::cout << "restoring input with move " << movePower << " and jump " << jumpPower << std::endl;
		}));
		scriptEmBinding.buildFunctions();

		auto parameterValues = std::vector<ska::NodeValue>{};
		parameterValues.push_back(1234);
		parameterValues.push_back(5566);
		
		auto parameterModule = ska::lang::ParameterModule(moduleConfiguration, parameterValues);
		auto iomodule = ska::lang::IOModule(moduleConfiguration);

		auto scriptCharacterCommands = ska::ScriptBridge{ scriptCache, "character_commands_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptCharacterCommands.bindFunction("jump", std::function<void(ska::Script&, int)>([](ska::Script&, int index) {
			std::cout << "jump ! " << index << std::endl;
		}));
		scriptCharacterCommands.bindFunction("move", std::function<void(ska::Script&, int, int, int)>([](ska::Script&, int direction, int power, int t) {
			std::cout << "move ! direction " << direction << " power " << power << " ??? " << t << std::endl;
		}));
		scriptCharacterCommands.buildFunctions();

		auto scriptCharacterBinding = ska::ScriptBridge{ scriptCache, "character_generator", typeBuilder, symbolsTypeUpdater, reservedKeywords };
		scriptCharacterBinding.import(parser, interpreter, { {"Character", "character"} });
		scriptCharacterBinding.bindGenericFunction("Gen", { "Character::Fcty" }, 
		std::function<ska::NodeValue(ska::Script&, std::vector<ska::NodeValue>)>([&](ska::Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto parametersValues = std::vector<ska::NodeValue>{};
			parametersValues.push_back("toto");
			return scriptCharacterBinding.callFunction(interpreter, "Character", "Fcty", std::move(parametersValues));
		}));
		scriptCharacterBinding.buildFunctions();

		auto scriptFileName = std::string{argv[1]};
		auto scriptName = scriptFileName.substr(0, scriptFileName.find_last_of('.'));
		auto executor = ska::Script{ scriptCache, "main", ska::Tokenizer{ reservedKeywords, 
		"var Script = import \"wd:" + scriptName + "\";"
		"var CharacterGenerator = import \"character_generator\";"
		"var ParametersGenerator = import \"std:std.native.parameter\";"
		"Script.run(CharacterGenerator.Gen(), ParametersGenerator.Gen(\"" + scriptName + "\"));"
		}.tokenize() };
	
		executor.parse(parser);
		interpreter.script(executor);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}


