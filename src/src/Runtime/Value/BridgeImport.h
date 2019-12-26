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
    std::string constructorMethod;

    Type type(const std::string& field) const {
      const auto* symbol = script->symbols()[field];
      if(symbol == nullptr) {
        throw std::runtime_error("unable to find field \"" + field + "\" in script \"" + script->name() + "\"");
      }
      return symbol->getType();
    }

    Type constructor() const {
      return type(constructorMethod);
    }

    std::string typeName(const std::string& field) const {
      const auto& nodeName = (*node)[0];
      assert(!nodeName.name().empty());
      return nodeName.name() + "::" + field;
    }

    std::string typeName() const {
      return typeName(constructorMethod + "()");
    }

    const auto& symbols() const {
      return script->symbols();
    }
  };
}
