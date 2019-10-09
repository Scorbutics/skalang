#pragma once

#include "NodeValue/ASTNodePtr.h"
#include "NodeValue/ScriptHandleAST.h"

namespace ska {
  struct BridgeImportRaw {
    ASTNodePtr node;
    ScriptHandleAST* script;
  };

  struct BridgeImport {
    ASTNode* node;
    ScriptHandleAST* script;

    Type type(const std::string& field) const {
      const auto* symbol = script->symbols()[field];
      if(symbol == nullptr) {
        throw std::runtime_error("unable to find field \"" + field + "\" in script \"" + script->name() + "\"");
      }
      return symbol->getType();
    }

    std::string typeName(const std::string& field) const {
      auto& nodeName = (*node)[0];
      assert(!nodeName.name().empty());
      return nodeName.name() + "::" + field;
    }

    const auto& symbols() const {
      return script->symbols();
    }
  };
}
