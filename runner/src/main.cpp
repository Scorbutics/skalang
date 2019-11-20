#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/DirectInterpreterTypes.h"
#include "Runtime/Service/ScriptProxy.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/ScriptNameBuilder.h"
#include "Interpreter/Interpreter.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

#include "Runtime/Value/InterpreterTypes.h"
#include "std/module/io/log.h"
#include "std/module/io/path.h"
#include "std/module/function/parameter.h"

ska::Script SmashAndDashSpecific(ska::lang::ModuleConfiguration<ska::Interpreter>& module, const std::string& scriptName) {
	auto scriptEmBinding = ska::ScriptBridge<ska::Interpreter>{ module.scriptCache, module.scriptCache.astCache, "em_lib", module.typeBuilder, module.symbolTableUpdater, module.reservedKeywords };
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
		scriptEmBinding.buildFunctions(module.interpreter);

		auto scriptCharacterCommands = ska::ScriptBridge<ska::Interpreter>{ module.scriptCache, module.scriptCache.astCache, "character_commands_lib", module.typeBuilder, module.symbolTableUpdater, module.reservedKeywords };
		scriptCharacterCommands.bindFunction("jump", std::function<void(int)>([](int index) {
			std::cout << "jump ! " << index << std::endl;
		}));
		scriptCharacterCommands.bindFunction("move", std::function<void(int, int, int)>([](int direction, int power, int t) {
			std::cout << "move ! direction " << direction << " power " << power << " ??? " << t << std::endl;
		}));
		scriptCharacterCommands.buildFunctions(module.interpreter);

		auto scriptCharacterBinding = ska::ScriptBridge<ska::Interpreter>{ module.scriptCache, module.scriptCache.astCache, "character_generator", module.typeBuilder, module.symbolTableUpdater, module.reservedKeywords };
		auto scriptCharacterProxy = ska::ScriptProxy<ska::Interpreter>{ scriptCharacterBinding };
		scriptCharacterBinding.import(module.parser, module.interpreter, {"Character", "character"});
		scriptCharacterBinding.bindGenericFunction("Gen", { "Character::Fcty" },
		std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
			auto parametersValues = std::vector<ska::NodeValue>{};
			parametersValues.push_back("toto");
			return scriptCharacterProxy.callFunction(module.interpreter, "Character", "Fcty", std::move(parametersValues));
		}));
		scriptCharacterBinding.buildFunctions(module.interpreter);

		auto executor = ska::Script{ module.scriptCache, "main", ska::Tokenizer{ module.reservedKeywords,
		"var Script = import \"wd:" + scriptName + "\";"
		"var CharacterGenerator = import \"bind:character_generator\";"
		"var ParametersGenerator = import \"bind:std.native.parameter\";"
		"Script.run(CharacterGenerator.Gen(), ParametersGenerator.Gen(\"" + scriptName + "\"));"
		}.tokenize() };
		return executor;
}

namespace ska {
	ska::lang::ParameterModule<ska::Interpreter> BasicParameterModuleBuilder(ska::lang::ModuleConfiguration<ska::Interpreter>& module, std::vector<ska::NodeValue>& parameters, int argc, char* argv[]) {
		for(auto i = 2; i < argc; i++) {
			parameters.push_back(std::make_shared<std::string>(argv[i]));
		}

		return ska::lang::ParameterModule(module, parameters);
	}

	ska::Script BasicProgramScriptStarter(ska::lang::ModuleConfiguration<ska::Interpreter>& module, char* argv[]) {
		auto scriptFileName = std::string{argv[1]};
		auto scriptName = scriptFileName.substr(0, scriptFileName.find_last_of('.'));

		auto executor = ska::Script{ module.scriptCache, "main", ska::Tokenizer{ module.reservedKeywords,
		"var Script = import \"wd:" + scriptName + "\";"
		"var ParametersGenerator = import \"bind:std.native.parameter\";"
		"Script.run(ParametersGenerator.Gen(\"" + scriptName + "\"));"
		}.tokenize() };
		return executor;
	}
}

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
  const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCache{};
	auto typeCrosser = ska::TypeCrosser{};
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser, typeCrosser };
	auto symbolsTypeUpdater = ska::SymbolTableUpdater {parser};
	auto typeChecker = ska::SemanticTypeChecker {parser, typeCrosser };
	auto interpreter = ska::Interpreter {reservedKeywords, typeCrosser };

	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::Interpreter> {scriptCache.astCache, scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords, parser, interpreter};

	try {

		auto logmodule = ska::lang::IOLogModule(moduleConfiguration);
		auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

		auto parameterValues = std::vector<ska::NodeValue>{};

		/*
			auto scriptFileName = std::string{argv[1]};
			auto scriptName = scriptFileName.substr(0, scriptFileName.find_last_of('.'));
			auto parameterModule = ska::lang::ParameterModule(moduleConfiguration, parameterValues);
			auto executor = SmashAndDashSpecific(moduleConfiguration, scriptName);
		*/


		auto parameterModule = ska::BasicParameterModuleBuilder(moduleConfiguration, parameterValues, argc, argv);
		auto executor = ska::BasicProgramScriptStarter(moduleConfiguration, argv);

		executor.astScript().parse(parser);
		interpreter.script(executor);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}


