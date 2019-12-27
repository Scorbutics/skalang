#pragma once
#include <string>
#include <vector>
#include "Runtime/Value/BridgeFunction.h"
#include "NodeValue/Type.h"
#include "NodeValue/Symbol.h"

namespace ska {
  struct BridgeField {
    using Callback = typename BridgeFunction::Callback;

    BridgeField() = default;
    //BridgeField(std::string name) : m_name(std::move(name)) {}
    //BridgeField(std::string name, std::vector<std::string> typeNames) : m_name(std::move(name)), typeNames(std::move(typeNames)) {}
    BridgeField(Type fullType) : type(std::move(fullType)) {
      assert(type.symbol() != nullptr && !type.symbol()->getName().empty());
    }

    const std::string& name() const {
      if(typeNames.empty()) {
        assert(type.symbol() != nullptr && !type.symbol()->getName().empty());
      }
      return typeNames.empty() ? type.symbol()->getName() : m_name;
    }

    void setName(std::string name) { m_name = std::move(name); }
  private:
    std::string m_name;

  public:
    std::vector<std::string> typeNames;
    Type type;
    Callback callback;
  };

  class BridgeFunctionData {
  public:
    BridgeFunctionData() = default;
    //BridgeFunctionData(std::string name) : m_function(std::move(name)) {}
    //BridgeFunctionData(std::string name, std::vector<std::string> typeNames) : m_function(std::move(name), std::move(typeNames)) {}
    BridgeFunctionData(Type fullType) : m_function(std::move(fullType)) {}
    BridgeFunctionData(BridgeField function) : m_function(std::move(function)) {}

    void bindField(BridgeField field) {
      m_fields.push_back(std::move(field));
    }

    bool hasFields() const { return !m_fields.empty(); }

    std::vector<BridgeField> stealFields() {
      return std::move(m_fields);
    }

    std::vector<std::string> stealTypeNames() { return std::move(m_function.typeNames); }

    const std::string& name() const { return m_function.name(); }

    const std::vector<std::string>& typeNames() const { return m_function.typeNames; }
    
    const Type& type() const { return m_function.type; }

  private:
    BridgeField m_function;

    std::vector<BridgeField> m_fields;
  };

}
