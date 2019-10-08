#pragma once
#include <vector>

#include "Runtime/Value/NodeValue.h"
#include "Interpreter/Service/ScriptProxy.h"

#include "std/module.h"

namespace ska {
    namespace lang {
        class ParameterModule : public Module {
        public:
            ParameterModule(ModuleConfiguration& config, const std::vector<NodeValue>& parameterValues);
            ~ParameterModule() override = default;
        private:
            ScriptProxy m_proxy;
            const std::vector<NodeValue>& m_parameters;
        };
    }
}