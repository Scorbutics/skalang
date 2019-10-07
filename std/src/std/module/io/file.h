#pragma once

#include "std/module.h"
#include "Interpreter/Service/ScriptBridge.h"

namespace ska {
    namespace lang {
        class IOFileModule : public Module {
        public:
            IOFileModule(ModuleConfiguration& config);
            ~IOFileModule() override = default;
        };
    }
}