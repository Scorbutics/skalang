#pragma once

namespace ska {
  class Interpreter;
  class Script;
  class ScriptCache;

  namespace bytecode {
    class Interpreter;
    class ScriptGenerationService;
  }

  template <class Interpreter>
  struct InterpreterTypes;

  template <>
  struct InterpreterTypes<ska::Interpreter> {
    using Script = ska::Script;
  };

  template <>
  struct InterpreterTypes<ska::Interpreter>;
}