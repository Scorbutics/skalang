#include <iostream>
#include <fstream>
#include <tuple>
#include "Config/LoggerConfigLang.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "BytecodeInterpreter/BytecodeScript.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Service/SymbolTableUpdater.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Serializer/BytecodeSerializer.h"

#include "Runtime/Value/InterpreterTypes.h"

#include "std/module/io/log.h"
#include "std/module/io/path.h"
#include "std/module/function/parameter.h"


static ska::bytecode::ScriptGenerationHelper BasicProgramScriptStarter(ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>& module, char* argv[]) {
	auto scriptFileName = std::string{ argv[1] };
	auto scriptName = scriptFileName.substr(0, scriptFileName.find_last_of('.'));

	const auto scriptStarter = "var Script = import \"wd:" + scriptName + "\";"
	"var ParametersGenerator = import \"bind:std.native.function.parameter\";"
	"Script.run(ParametersGenerator.Fcty(\"" + scriptName + "\"));";

	return { module.scriptCache, module.parser, "main", ska::Tokenizer{ module.reservedKeywords, scriptStarter}.tokenize() };
}

static ska::lang::ParameterModule<ska::bytecode::Interpreter> BasicParameterModuleBuilder(ska::lang::ModuleConfiguration<ska::bytecode::Interpreter>& module, std::vector<ska::NodeValue>& parameters, int argc, char* argv[]) {
	for(auto i = 2; i < argc; i++) {
		parameters.push_back(std::make_shared<std::string>(argv[i]));
	}

	return ska::lang::ParameterModule(module, parameters);
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cout << "No file name entered. Exiting..." << std::endl;
		return -1;
	}

	auto inputFile = std::ifstream{ std::string{argv[1]} };
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << std::endl;
		return -1;
	}

	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto typeCrosser = ska::TypeCrosser{};
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser, typeCrosser };
	auto symbolsTypeUpdater = ska::SymbolTableUpdater {parser};
	auto typeChecker = ska::SemanticTypeChecker {parser, typeCrosser };
	
	auto mainCache = ska::bytecode::ScriptCache {};
	auto generator = ska::bytecode::Generator{ reservedKeywords };
	auto interpreter = ska::bytecode::Interpreter { generator, reservedKeywords };

	auto moduleConfiguration = ska::lang::ModuleConfiguration<ska::bytecode::Interpreter> {
		mainCache.astCache,
		typeBuilder,
		symbolsTypeUpdater,
		typeChecker,
		reservedKeywords,
		parser,
		mainCache,
		interpreter
	};

	try {
		std::cout << "tototo" << std::endl;

		auto serializer = ska::bytecode::Serializer{};
		std::ifstream inputBytecode { "bytecode.out", std::ofstream::binary};
		if(!inputBytecode.fail()) {
			std::cout << "DESERIALIZATION STARTED" << std::endl;
			serializer.deserialize(mainCache, inputBytecode);
			inputBytecode.close();
		}


		auto logmodule = ska::lang::IOLogModule(moduleConfiguration);
		auto pathmodule = ska::lang::IOPathModule(moduleConfiguration);

		auto parameterValues = std::vector<ska::NodeValue>{};

		auto parameterModule = BasicParameterModuleBuilder(moduleConfiguration, parameterValues, argc, argv);
		auto script = BasicProgramScriptStarter(moduleConfiguration, argv);
		auto& gen = generator.generate(mainCache, std::move(script));

		if(inputBytecode.fail()) {
			std::cout << "SERIALIZATION STARTED" << std::endl;
			std::ofstream outputBytecode{ "bytecode.out", std::ofstream::binary };
			serializer.serialize(mainCache, outputBytecode);
			outputBytecode.close();
		}

		auto interpreted = interpreter.interpret(gen.id(), mainCache);
	} catch (std::exception& e) {
		std::cerr << "Error : " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

