#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "io.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"

ska::lang::IOModule::IOModule(ModuleConfiguration& config) :
	Module {config, "io_lib"} {
	m_bridge.bindFunction("printInt", std::function<void(int)>([](int value) {
		std::cout << value << std::endl;
	}));
	m_bridge.bindFunction("printString", std::function<void(std::string)>([](std::string value) {
		std::cout << value << std::endl;
	}));
	m_bridge.bindFunction("wait", std::function<void(int)>([](int value) {
		//TODO
		std::cout << "wait " << value << std::endl;
	}));
	m_bridge.buildFunctions();
}

