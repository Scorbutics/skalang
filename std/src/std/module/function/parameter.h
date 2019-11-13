#pragma once
#include <vector>

#include "Runtime/Value/NodeValue.h"
#include "Interpreter/Service/ScriptProxy.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/Interpreter.h"

#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class ParameterModule : public Module<Interpreter> {
        public:
            ParameterModule(ModuleConfiguration<Interpreter>& config, const std::vector<NodeValue>& parameterValues) :
                Module<Interpreter> { config, "std.native.parameter" },
				m_proxy(Module<Interpreter>::m_bridge),
				m_parameters(parameterValues) {
                auto parametersImport = Module<Interpreter>::m_bridge.import(config.parser, config.interpreter, {"Parameters", "std:std.function.parameters"});

                Module<Interpreter>::m_bridge.bindGenericFunction("Gen", { "string", parametersImport.typeName("Fcty") },
					BridgeFunction::Callback ([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
                    
					auto result = m_proxy.createMemory(parametersImport);
                    result.replace("asInt", std::make_unique<ska::BridgeFunction>(
                        std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
                        const std::size_t index = params[0].nodeval<long>();
                        return m_parameters.size() > index ? static_cast<long>(m_parameters[index].convertNumeric()) : -1;
                    })));
                    result.replace("asString", std::make_unique<ska::BridgeFunction>(
                        std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
                        const std::size_t index = params[0].nodeval<long>();
                        return std::make_shared<std::string>(std::move(m_parameters.size() > index ? m_parameters[index].convertString() : ""));
                    })));
                    result.replace("size", std::make_unique<ska::BridgeFunction>(
                        std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) {
                        return static_cast<long>(m_parameters.size());
                    })));

                    return result.memory;
                }));
                Module<Interpreter>::m_bridge.buildFunctions();
            }

            ~ParameterModule() override = default;
        private:
            ScriptProxy<Interpreter> m_proxy;
            const std::vector<NodeValue>& m_parameters;
        };
    }
}