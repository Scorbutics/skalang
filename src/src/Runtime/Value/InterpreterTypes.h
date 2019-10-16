#pragma once

namespace ska {
  class Interpreter;
  class Script;
  class ScriptCache;

  namespace bytecode {
    class Interpreter;
    struct Storage;
    struct ScriptCache;
  }

  template <class Interpreter>
  struct InterpreterTypes;

  template <>
  struct InterpreterTypes<ska::Interpreter> {
    using Script = ska::Script;
    using ScriptCache = ska::ScriptCache;
  };

  template <>
  struct InterpreterTypes<ska::bytecode::Interpreter> {
    using Script = ska::bytecode::Storage;
    using ScriptCache = ska::bytecode::ScriptCache;
  };
}
