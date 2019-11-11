#pragma once

namespace ska {
  class Interpreter;
  class Script;
  class ScriptCache;
  using MemoryTablePtr = std::shared_ptr<MemoryTable>;

  namespace bytecode {
    class Interpreter;
    class Script;
    struct ScriptCache;
	using PlainMemoryTable = std::vector<NodeValue>;
  }

  template <class Interpreter>
  struct InterpreterTypes;

  template <>
  struct InterpreterTypes<ska::Interpreter> {
    using Script = ska::Script;
    using ScriptCache = ska::ScriptCache;
	using Memory = ska::MemoryTablePtr;
  };

  template <>
  struct InterpreterTypes<ska::bytecode::Interpreter> {
    using Script = ska::bytecode::Script;
    using ScriptCache = ska::bytecode::ScriptCache;
	using Memory = ska::bytecode::PlainMemoryTable;
  };
}
