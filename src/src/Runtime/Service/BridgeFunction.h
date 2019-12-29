#pragma once
#include <string>
#include <vector>
#include "Runtime/Value/NativeFunction.h"
#include "NodeValue/Type.h"
#include "NodeValue/Symbol.h"

namespace ska {
  struct BridgeField {
    using Callback = typename NativeFunction::Callback;

    BridgeField() = default;
    BridgeField(Type fullType) : type(std::move(fullType)) {
      assert(type.symbol() != nullptr && !type.symbol()->getName().empty());
    }

    const std::string& name() const {
        assert(type.symbol() != nullptr && !type.symbol()->getName().empty());      
        return type.symbol()->getName();
    }

  public:
    Type type;
    Callback callback;
  };

  class BridgeFunction {
  public:
    BridgeFunction() = default;
    BridgeFunction(Type fullType) : m_function(std::move(fullType)) {}
    BridgeFunction(BridgeField function) : m_function(std::move(function)) {}

    void bindField(BridgeField field) {
      m_fields.push_back(std::move(field));
    }

    bool hasFields() const { return !m_fields.empty(); }

    std::vector<BridgeField> stealFields() {
      return std::move(m_fields);
    }

    const std::string& name() const { return m_function.name(); }
    
    const Type& type() const { return m_function.type; }

  private:
    BridgeField m_function;

    std::vector<BridgeField> m_fields;
  };

}
