#pragma once

#include "std/module.h"
#include "Interpreter/Service/ScriptBridge.h"

namespace ska {
    namespace lang {
        class IOLogModule : public Module {
        public:
            IOLogModule(ModuleConfiguration& config);
            ~IOLogModule() override = default;
        };
    }
}