#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "io.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"

void ska::lang::UseModuleIO(ModuleConfiguration config) {
	auto scriptBinding = ska::ScriptBridge{ config.scriptCache, "io_lib", config.typeBuilder, config.symbolTableTypeUpdater, config.reservedKeywords };
	scriptBinding.bindFunction("printInt", std::function<void(int)>([](int value) {
		std::cout << value << std::endl;
	}));
	scriptBinding.bindFunction("printString", std::function<void(std::string)>([](std::string value) {
		std::cout << value << std::endl;
	}));
	scriptBinding.bindFunction("wait", std::function<void(int)>([](int value) {
		//TODO
		std::cout << "wait " << value << std::endl;
	}));
	scriptBinding.buildFunctions();
}

