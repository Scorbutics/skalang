#pragma once
#include <iostream>
#include "std/module.h"

namespace ska {
    namespace lang {
        template <class Interpreter>
        class IOLogModule : public Module<Interpreter> {
        public:
            IOLogModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.io.log", "std:std.io.log"},
                m_constructor(BridgeConstructor<Interpreter> { Module<Interpreter>::m_bridge, "" }) {

                m_constructor.bindField("printString", [&](std::vector<ska::NodeValue> params) {
                    std::cout << *params[0].template nodeval<StringShared>() << std::endl;
                    return NodeValue {};
                });

                m_constructor.bindField("printInt", [&](std::vector<ska::NodeValue> params) {
                    std::cout << params[0].template nodeval<long>() << std::endl;
                    return NodeValue{};
                });

                m_constructor.generate();
            }
            ~IOLogModule() override = default;
        private:
            BridgeConstructor<Interpreter> m_constructor;
        };
    }
}