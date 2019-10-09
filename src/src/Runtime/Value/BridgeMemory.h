#pragma once
#include <memory>
#include "NodeValue/Type.h"

namespace ska {
    struct BridgeFunction;
    using BridgeFunctionPtr = std::shared_ptr<BridgeFunction>;

    template <class Object>
    struct BridgeMemory {
        Object memory;
        Type type;
        template <class Value>
        void replace(const std::string& field, Value&& obj) {
            memory->emplace(field, std::forward<Value>(obj));
        }
    };
}