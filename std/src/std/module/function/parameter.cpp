#include "Config/LoggerConfigLang.h"
#include "parameter.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/Interpreter.h"

ska::lang::ParameterModule::ParameterModule(ModuleConfiguration& config, const std::vector<NodeValue>& parameterValues) :
    Module { config, "std.native.parameter" },
    m_parameters(parameterValues),
    m_proxy { m_bridge } {
    auto parametersImport = m_bridge.import(config.parser, config.interpreter, {"Parameters", "std:std.function.parameters"});
    m_bridge.bindGenericFunction("Gen", { "string", parametersImport.typeName("Fcty") },
    std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
        auto result = m_proxy.createMemory(parametersImport.type("Fcty"));
        result.replace("asInt", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<long>();
            return m_parameters.size() > index ? static_cast<long>(m_parameters[index].convertNumeric()) : -1;
        })));
        result.replace("asString", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            const auto index = params[0].nodeval<long>();
            return std::make_shared<std::string>(std::move(m_parameters.size() > index ? m_parameters[index].convertString() : ""));
        })));
        result.replace("size", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
            return static_cast<long>(m_parameters.size());
        })));
        return result.memory;
    }));
    m_bridge.buildFunctions();
}

