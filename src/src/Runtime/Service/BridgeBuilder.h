#pragma once

#include <unordered_map>

#include "NodeValue/ASTNodePtr.h"
#include "NodeValue/ScriptHandleAST.h"

#include "ScriptBinding.h"

namespace ska {
  class SymbolTable;
	class StatementParser;

  class BridgeBuilderASTTemplateLooker {
  public:
    BridgeBuilderASTTemplateLooker(const ScriptAST& templateParent, std::string constructorName);

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
	class BridgeBuilder {
  public:
      BridgeBuilder(ScriptBinding<Interpreter>& bindingTarget, std::string constructorName = ""):
      m_templateLooker(bindingTarget.templateScript(), std::move(constructorName)),
      m_bindingTarget(bindingTarget),
        m_constructor{ BridgeFunction{ BridgeField{m_templateLooker.constructor()} } }{
    }

    void generate() {
      m_bindingTarget.buildFunctions(m_constructor);
    }

    void bindField(std::string name, NativeFunction::Callback binding) {
      m_bindingTarget.bindFunction(m_templateLooker.field(name), std::move(binding));
    }

    template <class T>
    T& param(std::size_t paramIndex) {
        return m_constructor.template param<T>(paramIndex);
    }

  private:
    BridgeBuilderASTTemplateLooker m_templateLooker;
    ScriptBinding<Interpreter>& m_bindingTarget;
    BridgeFunction m_constructor;
  };
}