#pragma once
#include <string>
#include <vector>
#include "Runtime/Value/NativeFunction.h"
#include "Runtime/Value/NodeValue.h"

namespace ska {
  class Symbol;
  struct BridgeField {
    using Callback = typename NativeFunction::Callback;

    BridgeField() = default;
    ~BridgeField() = default;

    BridgeField(const Symbol& fullType) : symbol(&fullType) {}

    std::string name() const;

  public:
    const Symbol* symbol = nullptr;
    Callback callback;
  };

  class BridgeFunction {
  public:
    BridgeFunction() = default;
    BridgeFunction(const Symbol& fullType) : m_function(fullType) {}
    BridgeFunction(BridgeField function) : m_function(std::move(function)) {}

    void bindField(BridgeField field) {
      m_fields.push_back(std::move(field));
    }

    bool hasFields() const { return !m_fields.empty(); }

    std::vector<BridgeFunction> makeFunctions() const;

    std::string name() const { return m_function.name(); }
    
    const Symbol& symbol() const { if(m_function.symbol == nullptr) throw std::runtime_error("bad function symbol"); return *m_function.symbol; }

    bool isVoid() const;

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
