#pragma once
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Value/VectorMapped.h"
#include "Runtime/Value/InterpreterTypes.h"



namespace ska {
  namespace bytecode {
    template <class T>
    using SharedDeque = std::shared_ptr<std::deque<T>>;

    class RuntimeMemory  {
    public:
      RuntimeMemory(NodeValueArray table) : m_container(table) {}
      ~RuntimeMemory() = default;

      NodeValue value() const {
        return m_container.underlying();
      }
    private:
      container_mapped<SharedDeque, std::string, NodeValue> m_container;
    };
  }

  template <>
  struct InterpreterTypes<ska::bytecode::Interpreter> {
    using Script = ska::bytecode::Script;
    using ScriptCache = ska::bytecode::ScriptCache;
    using Memory = ska::bytecode::RuntimeMemory;
  };
}
