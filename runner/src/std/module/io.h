#pragma once

#include "std/module.h"
#include "Interpreter/ScriptBinding.h"

namespace ska {
    namespace lang {
        class IOModule : public Module {
        public:
            IOModule(ModuleConfiguration& config);
            ~IOModule() override = default;
        };
    }
}