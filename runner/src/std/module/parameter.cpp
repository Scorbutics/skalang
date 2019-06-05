#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "parameter.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"

ska::lang::ParameterModule::ParameterModule(ModuleConfiguration& config) :
    m_bridge { config.scriptCache, "parameter_lib", config.typeBuilder, config.symbolTableTypeUpdater, config.reservedKeywords } {
    m_bridge.import(config.parser, config.interpreter, { {"Parameters", "parameters"} });
    m_bridge.bindGenericFunction("Gen", { "Parameters::Fcty" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
        auto result = m_bridge.createMemory();
        result->emplace("asInt", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            auto index = params[0].nodeval<int>();
            std::cout << "accessing parameter " << index << " as int" << std::endl;
            //TODO conversion from other types
            return ska::NodeValue{ index };
        })));
        result->emplace("asString", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            std::cout << "accessing parameter " << params[0].nodeval<int>() << " as string" << std::endl;
            //TODO conversion
            return ska::NodeValue{ "toto" };
        })));
        return result;
    }));
    m_bridge.buildFunctions();
}

