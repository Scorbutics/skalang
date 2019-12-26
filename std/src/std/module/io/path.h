#pragma once
#include <iostream>
#include "std/module.h"
#include "Runtime/Value/ObjectMemory.h"
#include "Base/IO/Files/FileUtils.h"
#include "Runtime/Service/ScriptProxy.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOPathModule : public Module<Interpreter> {
        using Object = typename InterpreterTypes<Interpreter>::Memory;
        public:
            IOPathModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> { config, "std.native.io.path", "std:std.io.path" },
                m_proxy { Module<Interpreter>::m_bridge }  {

                auto importTemplate = Module<Interpreter>::m_bridge.generateTemplate("Fcty");
                Module<Interpreter>::m_bridge.bindGenericFunction(importTemplate, "canonical", { "void", "string" },
                std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
                        std::cout << "LOULOULOULOU" << std::endl;
                        return std::make_shared<std::string>(std::move(FileUtils::getCanonicalPath(/*importTemplate.template param<StringShared>(0) */ "")));
                }));

                Module<Interpreter>::m_bridge.buildFunctions();
            }
            ~IOPathModule() override = default;
        private:
            ScriptProxy<Interpreter> m_proxy;
        };
    }
}