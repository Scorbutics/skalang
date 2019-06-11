#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "parameter.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"

ska::lang::ParameterModule::ParameterModule(ModuleConfiguration& config, const std::vector<NodeValue>& parameterValues) :
    Module { config, "parameter_lib" },
    m_parameters(parameterValues) {
    m_bridge.import(config.parser, config.interpreter, { {"Parameters", "parameters"} });
    m_bridge.bindGenericFunction("Gen", { "string", "Parameters::Fcty" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
        auto result = m_bridge.createMemory();
        static const auto scriptName = params[0].nodeval<std::string>();
        //result->emplace("_scriptname_", std::move(params[0]));
        result->emplace("asInt", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<int>();
            //const auto* scriptNameNode = ()("_scriptname_").first;
            std::cout << "accessing parameter " << index << " as int in script " << scriptName << std::endl;
            return static_cast<int>(m_parameters[index].convertNumeric());
        })));
        result->emplace("asString", std::make_unique<ska::BridgeFunction>(std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<int>();
            std::cout << "accessing parameter " << index << " as string" << std::endl;
            return m_parameters[index].convertString();
        })));
        return result;
    }));
    m_bridge.buildFunctions();
}

