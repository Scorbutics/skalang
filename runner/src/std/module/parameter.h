#pragma once

#include "std/module.h"
#include "Interpreter/ScriptBinding.h"

namespace ska {
    namespace lang {
        class ParameterModule {
        public:
            ParameterModule(ModuleConfiguration& config);
        private:
            ScriptBridge m_bridge;
        };
    }
}