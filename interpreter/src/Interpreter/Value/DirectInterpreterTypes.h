#pragma once
#include "Runtime/Value/InterpreterTypes.h"
#include "Script.h"
#include "Interpreter/Interpreter.h"

namespace ska {

  class RuntimeMemory {
  public:
    RuntimeMemory(MemoryTablePtr table) : m_container(table) {}
    RuntimeMemory() = default;
    ~RuntimeMemory() = default;

    NodeValue value() const {
      return m_container;
    }

    template <class Value>
    void emplace(const std::string& field, Value&& value) {
      m_container->emplace(field, std::forward<Value>(value));
    }

  private:
    MemoryTablePtr m_container;
  };

  template <>
  struct InterpreterTypes<ska::Interpreter> {
    using Script = ska::Script;
    using ScriptCache = ska::ScriptCache;
    using Memory = ska::RuntimeMemory;
  };
}