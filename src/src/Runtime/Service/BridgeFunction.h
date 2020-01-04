#pragma once
#include <string>
#include <vector>
#include "Runtime/Value/NativeFunction.h"
#include "NodeValue/Type.h"
#include "NodeValue/Symbol.h"
#include "Runtime/Value/NodeValue.h"

namespace ska {
  struct BridgeField {
    using Callback = typename NativeFunction::Callback;

    BridgeField() = default;
    ~BridgeField() = default;

    BridgeField(Type fullType) : type(std::move(fullType)) {}

    std::string name() const {
        return type.symbol() != nullptr ? type.symbol()->getName() : "";
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

    std::vector<BridgeFunction> makeFunctions() const {
        auto result = std::vector<BridgeFunction> {};
        if (m_fields.empty()) {
            return result;
        }
        result.resize(m_fields.size());
        std::transform(m_fields.begin(), m_fields.end(), result.begin(), [](const auto& field) {
            return BridgeFunction{ field.type };
        });
        return result;
    }

    std::string name() const { return m_function.name(); }
    
    const Type& type() const { return m_function.type; }

    const BridgeField::Callback& callback() const { return m_function.callback; }
    const std::vector<BridgeField>& fields() const { return m_fields; }

    void setAdditionalParams(std::vector<NodeValue> params) { m_additionalParamValues = std::move(params); }
    template <class T>
    T& param(std::size_t index) {
        return m_additionalParamValues[index].nodeval<T>();
    }

  private:
    BridgeField m_function;
    std::vector<NodeValue> m_additionalParamValues;
    std::vector<BridgeField> m_fields;
  };

}
