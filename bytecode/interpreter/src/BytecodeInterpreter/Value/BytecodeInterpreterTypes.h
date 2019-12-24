#pragma once
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Value/VectorMapped.h"
#include "Runtime/Value/InterpreterTypes.h"



namespace ska {
  namespace bytecode {
    class Script;
    template <class T>
    using SharedDeque = std::shared_ptr<std::deque<T>>;

    class RuntimeMemory  {
    public:
      static auto buildFromVector(PlainMemoryTable table) {
        auto result = std::make_shared<std::deque<NodeValue>>();
        std::move(
            std::begin(table),
            std::end(table),
            std::back_inserter(*result)
        );
        return result;
      }

      RuntimeMemory(Operand root) {
        /*
        TODO
        */
      }
      RuntimeMemory(PlainMemoryTable table) : m_container(buildFromVector(std::move(table))) {}
      RuntimeMemory(NodeValueArray table) : m_container(std::move(table)) {}
      ~RuntimeMemory() = default;

      NodeValue value() const {
        return m_container.underlying();
      }

      template <class Openrand>
      void emplace(const std::string& field, Openrand&& value) {
        m_container.emplace(field, std::forward<Openrand>(value));
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
