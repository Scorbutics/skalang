#pragma once

#include "std/module.h"
#include "Interpreter/Service/ScriptProxy.h"

namespace ska {
    namespace lang {
        class IOPathModule : public Module {
        public:
            IOPathModule(ModuleConfiguration& config);
            ~IOPathModule() override = default;
        private:
            ScriptProxy m_proxy;
        };
    }
}