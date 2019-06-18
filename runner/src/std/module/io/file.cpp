#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "file.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"

ska::lang::IOFileModule::IOFileModule(ModuleConfiguration& config) :
	Module {config, "std.native.io.file"} {
	m_bridge.import(config.parser, config.interpreter, { {"File", "std:std.io.file"} });
	m_bridge.bindGenericFunction("Open", { "string", "File::Fcty()" },
    	std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
		auto file = m_bridge.callFunction(config.interpreter, "File", "Fcty", std::move(params));
		auto& memFile = file.nodeval<ska::ObjectMemory>();

		//TODO memory manipulation here
		//memCharacter->emplace("name", std::string{ "titito" });

		return ska::NodeValue{ std::move(memFile) };
    }));
	m_bridge.buildFunctions();
}

