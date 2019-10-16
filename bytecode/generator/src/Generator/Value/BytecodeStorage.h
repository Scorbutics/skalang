#pragma once

#include "BytecodeScriptCache.h"

namespace ska {
  namespace bytecode {

    struct Storage {
      Storage() = default;
      Storage(ScriptGenerationService service) : service(std::move(service)) {}
      Storage(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens);
      Storage(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName);

      Storage(const Storage&) = delete;
      Storage(Storage&&) = default;

      Storage& operator=(const Storage&) = delete;
      Storage& operator=(Storage&&) = default;

      ScriptAST& astScript();
      void memoryFromBridge(std::vector<BridgeFunctionPtr> bindings);

      ~Storage() = default;

      ScriptGenerationService service;
      ScriptGenerationOutput output;
    private:
      std::optional<ScriptAST> m_ast;
    };

  }
}
