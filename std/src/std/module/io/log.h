#pragma once

#include "std/module.h"
#include "Interpreter/Service/ScriptBinding.h"

namespace ska {
    namespace lang {
        class IOLogModule : public Module {
        public:
            IOLogModule(ModuleConfiguration& config);
            ~IOLogModule() override = default;
        };
    }
}