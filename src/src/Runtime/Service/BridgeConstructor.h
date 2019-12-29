#pragma once

#include <unordered_map>

#include "NodeValue/ASTNodePtr.h"
#include "NodeValue/ScriptHandleAST.h"

#include "ScriptBinding.h"

namespace ska {
  class SymbolTable;
	class StatementParser;

  class BridgeConstructorASTTemplateLooker {
  public:
    BridgeConstructorASTTemplateLooker(const ScriptAST& templateParent, std::string constructorName);

    Type variable(const std::string& name) const;
    Type field(const std::string& name) const;
    bool hasField(const std::string& name) const;
    Type constructor() const;

  private:
    std::string m_name;
    ScriptHandleAST* m_parent;
    const Symbol* m_template;
  };

  template <class Interpreter>
	class BridgeConstructor {
  public:
    BridgeConstructor(ScriptBinding<Interpreter>& bindingTarget, std::string constructorName):
      m_templateLooker(bindingTarget.templateScript(), std::move(constructorName)),
      m_bindingTarget(bindingTarget) {
    }

    void generate() {
      m_bindingTarget.buildFunctions(BridgeField{m_templateLooker.constructor()});
    }

    void bindField(std::string name, decltype(NativeFunction::function) binding) {
      m_bindingTarget.bindFunction(m_templateLooker.field(name), std::move(binding));
    }

  private:
    BridgeConstructorASTTemplateLooker m_templateLooker;
    ScriptBinding<Interpreter>& m_bindingTarget;
  };
}