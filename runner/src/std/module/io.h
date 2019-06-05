#pragma once

#include "std/module.h"
#include "Interpreter/ScriptBinding.h"

namespace ska {
    namespace lang {
        class IOModule {
        public:
            IOModule(ModuleConfiguration& config);
        private:
            ScriptBridge m_bridge;
        };
    }
}