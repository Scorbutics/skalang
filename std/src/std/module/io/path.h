#pragma once

#include "std/module.h"
#include "Interpreter/ScriptBinding.h"

namespace ska {
    namespace lang {
        class IOPathModule : public Module {
        public:
            IOPathModule(ModuleConfiguration& config);
            ~IOPathModule() override = default;
        };
    }
}