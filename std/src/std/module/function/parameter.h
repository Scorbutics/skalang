#pragma once
#include <vector>

#include "Runtime/Value/NodeValue.h"

#include "std/module.h"

namespace ska {
    namespace lang {
        class ParameterModule : public Module {
        public:
            ParameterModule(ModuleConfiguration& config, const std::vector<NodeValue>& parameterValues);
            ~ParameterModule() override = default;
        private:
            const std::vector<NodeValue>& m_parameters;
        };
    }
}