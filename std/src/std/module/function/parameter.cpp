#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "parameter.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"

ska::lang::ParameterModule::ParameterModule(ModuleConfiguration& config, const std::vector<NodeValue>& parameterValues) :
    Module { config, "std.native.parameter" },
    m_parameters(parameterValues) {
    m_bridge.import(config.parser, config.interpreter, { {"Parameters", "std:std.function.parameters"} });
    m_bridge.bindGenericFunction("Gen", { "string", "Parameters::Fcty" },
    std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script&, std::vector<ska::NodeValue> params) -> ska::NodeValue {
        auto result = m_bridge.createMemory();
        result->emplace("asInt", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script& caller, std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<int>();
            std::cout << "accessing parameter " << index << " as int in script " << caller.name() << std::endl;
            return m_parameters.size() > index ? static_cast<int>(m_parameters[index].convertNumeric()) : -1;
        })));
        result->emplace("asString", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script&, std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<int>();
            std::cout << "accessing parameter " << index << " as string" << std::endl;
            return std::make_shared<std::string>(std::move(m_parameters.size() > index ? m_parameters[index].convertString() : ""));
        })));
        result->emplace("size", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script&, std::vector<ska::NodeValue> params) {
            return static_cast<int>(m_parameters.size());
        })));
        return result;
    }));
    m_bridge.buildFunctions();
}

