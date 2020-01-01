#pragma once
#include <iostream>
#include "std/module.h"
#include "Runtime/Value/ObjectMemory.h"
#include "Base/IO/Files/FileUtils.h"
#include "Runtime/Service/ScriptProxy.h"
#include "Runtime/Service/BridgeConstructor.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOPathModule : public Module<Interpreter> {
        using Object = typename InterpreterTypes<Interpreter>::Memory;
        public:
            IOPathModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> { config, "std.native.io.path", "std:std.io.path" },
                m_constructor(BridgeConstructor<Interpreter> { Module<Interpreter>::m_bridge, "Fcty" }) {

                m_constructor.bindField("canonical", std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
                    return std::make_shared<std::string>(FileUtils::getCanonicalPath(*m_constructor.template param<StringShared>(0)));
                }));

                m_constructor.generate();
            }
            ~IOPathModule() override = default;
        private:
            BridgeConstructor<Interpreter> m_constructor;
        };
    }
}
