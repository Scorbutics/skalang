#pragma once

#include "std/module.h"
#include "Interpreter/Service/ScriptBridge.h"

namespace ska {
    namespace lang {
        class IOPathModule : public Module {
        public:
            IOPathModule(ModuleConfiguration& config);
            ~IOPathModule() override = default;
        };
    }
}