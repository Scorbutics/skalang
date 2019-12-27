#pragma once
#include "Runtime/Value/BridgeFunction.h"
#include "NodeValue/Type.h"
#include "NodeValue/Symbol.h"

namespace ska {

  struct BridgeField {
    using Callback = typename BridgeFunction::Callback;
    std::string name;
    Type type;
    Callback callback;
  };

  class BridgeFunctionData {
  public:
    BridgeFunctionData() = default;
    BridgeFunctionData(std::string name, std::vector<std::string> typeNames) : m_name(std::move(name)), m_typeNames(std::move(typeNames)) {}
    BridgeFunctionData(Type fullType) : m_fullType(std::move(fullType)) {
      assert(m_fullType.symbol() != nullptr && !m_fullType.symbol()->getName().empty());
    }

    void bindField(BridgeField field) {
      m_fields.push_back(std::move(field));
    }

    std::vector<BridgeField> stealFields() {
      return std::move(m_fields);
    }

    std::vector<std::string> stealTypeNames() { return std::move(m_typeNames); }

    const std::string& name() const { return m_typeNames.empty() ? m_fullType.symbol()->getName() : m_name; }

    const std::vector<std::string>& typeNames() const { return m_typeNames; }
    
    const Type& type() const { return m_fullType; }

  private:
    std::string m_name;
    std::vector<std::string> m_typeNames;

    Type m_fullType;

    std::vector<BridgeField> m_fields;
  };
}