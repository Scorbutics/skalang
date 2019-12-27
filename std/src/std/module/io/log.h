#pragma once

#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOLogModule : public Module<Interpreter> {
        public:
            IOLogModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.io.log", ""} {
                /*
                Module<Interpreter>::m_bridge.bindFunction("printInt", std::function<void(int)>([](int value) {
                    std::cout << value << std::endl;
                }));
                Module<Interpreter>::m_bridge.bindFunction("printString", std::function<void(ska::StringShared)>([](ska::StringShared value) {
                    std::cout << *value << std::endl;
                }));
                Module<Interpreter>::m_bridge.bindFunction("wait", std::function<void(int)>([](int value) {
                    //TODO
                    std::cout << "wait " << value << std::endl;
                }));
                */
                Module<Interpreter>::m_bridge.buildFunctions({});
            }
            ~IOLogModule() override = default;
        };
    }
}