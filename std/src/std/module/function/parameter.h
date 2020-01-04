#pragma once
#include <vector>

#include "Runtime/Value/NodeValue.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Runtime/Service/BridgeConstructor.h"

#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class ParameterModule : public Module<Interpreter> {
        public:
            ParameterModule(ModuleConfiguration<Interpreter>& config, const std::vector<NodeValue>& parameterValues) :
                Module<Interpreter> { config, "std.native.function.parameter", "std:std.function.parameter" },
                m_constructor(BridgeConstructor<Interpreter> { Module<Interpreter>::m_bridge, "Fcty" }),
				m_parameters(parameterValues) {

                m_constructor.bindField("asInt", [&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
                    const std::size_t index = params[0].nodeval<long>();
                    return m_parameters.size() > index ? static_cast<long>(m_parameters[index].convertNumeric()) : -1;
                });
                m_constructor.bindField("asString", [&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
                    const std::size_t index = params[0].nodeval<long>();
                    return std::make_shared<std::string>(m_parameters.size() > index ? m_parameters[index].convertString() : "");
                });
                m_constructor.bindField("size", [&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
                    return static_cast<long>(m_parameters.size());
                });
                m_constructor.generate();
            }

            ~ParameterModule() override = default;
        private:
            const std::vector<NodeValue>& m_parameters;
            BridgeConstructor<Interpreter> m_constructor;
        };
    }
}