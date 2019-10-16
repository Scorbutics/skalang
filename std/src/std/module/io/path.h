#pragma once

#include "std/module.h"
#include "Runtime/Value/ObjectMemory.h"
#include "Base/IO/Files/FileUtils.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOPathModule : public Module<Interpreter> {
        public:
            IOPathModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.io.path"}/*,
                m_proxy { Module<Interpreter>::m_bridge } */ {
                auto importPath = Module<Interpreter>::m_bridge.import(config.parser, config.interpreter, {"Path", "std:std.io.path"});
                Module<Interpreter>::m_bridge.bindGenericFunction("Build", { "string", importPath.typeName("Fcty()") },
                    std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
                    /*auto path = m_proxy.callFunction(config.interpreter, "Path", "Fcty", std::move(buildParams));
                    auto& memPath = path.template nodeval<ska::ObjectMemory>();

                    //Query input parameters
                    const auto* memberPathValue = (*memPath)["path"].first;
                    assert(memberPathValue != nullptr);
                    auto pathStr = std::move(memberPathValue->template nodeval<StringShared>());

                    //Build output object
                    memPath->emplace("canonical", std::make_unique<ska::BridgeFunction>(
                        std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&, pathStr(std::move(pathStr))](std::vector<ska::NodeValue> unused) {
                        return std::make_shared<std::string>(std::move(FileUtils::getCanonicalPath(*pathStr)));
                    })));
                    */
                    return ska::NodeValue{ /*std::move(memPath)*/ };
                }));
                Module<Interpreter>::m_bridge.buildFunctions();
            }
            ~IOPathModule() override = default;
        private:
            //ScriptProxy m_proxy;
        };
    }
}