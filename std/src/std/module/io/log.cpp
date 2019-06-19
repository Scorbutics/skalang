#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "log.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"

ska::lang::IOLogModule::IOLogModule(ModuleConfiguration& config) :
	Module {config, "std.native.io.log"} {
	m_bridge.bindFunction("printInt", std::function<void(Script&, int)>([](Script&, int value) {
		std::cout << value << std::endl;
	}));
	m_bridge.bindFunction("printString", std::function<void(Script&, std::string)>([](Script&, std::string value) {
		std::cout << value << std::endl;
	}));
	m_bridge.bindFunction("wait", std::function<void(Script&, int)>([](Script&, int value) {
		//TODO
		std::cout << "wait " << value << std::endl;
	}));
	m_bridge.buildFunctions();
}

