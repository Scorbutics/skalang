#pragma once
#include <memory>
#include "NodeValue/Type.h"
#include "Service/SymbolTable.h"

namespace ska {
    struct BridgeFunction;
    using BridgeFunctionPtr = std::shared_ptr<BridgeFunction>;

    template <class Object>
    struct BridgeMemory {
        Object memory;
        const SymbolTable& symbols;

        template <class Value>
        void replace(const std::string& field, Value&& obj) {
            if(symbols[field] == nullptr) {
                throw std::runtime_error("unable to replace the field \"" + field + "\" while binding this script because it is not found as declared in any external script");
            }
            memory->emplace(field, std::forward<Value>(obj));
        }
    };
}