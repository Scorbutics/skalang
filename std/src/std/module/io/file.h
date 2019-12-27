#pragma once

#include "std/module.h"
#include "Runtime/Service/ScriptProxy.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOFileModule : public Module<Interpreter> {
        public:
            IOFileModule(ModuleConfiguration<Interpreter>& config)  :
                Module<Interpreter> {config, "std.native.io.file"},
                m_proxy { Module<Interpreter>::m_bridge } {
                auto importFile = Module<Interpreter>::m_bridge.import(config.parser, config.interpreter, {"File", "std:std.io.file"});
                Module<Interpreter>::m_bridge.bindGenericFunction("Open", { "string", importFile.typeName("Fcty()") },
                    std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
                    auto file = m_proxy.callFunction(config.interpreter, "File", "Fcty", std::move(params));
                    //auto& memFile = file.nodeval<ska::ObjectMemory>();

                    //TODO memory manipulation here
                    //memCharacter->emplace("name", std::string{ "titito" });

                    return ska::NodeValue{ /*std::move(memFile)*/ };
                }));
                Module<Interpreter>::m_bridge.buildFunctions({});
            }

            ~IOFileModule() override = default;
        private:
            ScriptProxy<Interpreter> m_proxy;
        };
    }
}