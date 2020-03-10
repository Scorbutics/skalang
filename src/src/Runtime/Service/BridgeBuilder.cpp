#include "Config/LoggerConfigLang.h"
#include "BridgeBuilder.h"
#include "NodeValue/ScriptAST.h"
#include "Service/ASTFactory.h"

ska::BridgeBuilderASTTemplateLooker::BridgeBuilderASTTemplateLooker(const ScriptAST& templateParent, std::string constructorName) :
  m_parent(templateParent.handle()),
  m_name(std::move(constructorName)),
  m_template(constructor().symbol()) {
}

ska::Type ska::BridgeBuilderASTTemplateLooker::variable(const std::string& name) const {
  const auto* symbol = m_parent->symbols()[name];
  if(symbol == nullptr) {
    throw std::runtime_error("unable to find variable \"" + name + "\" in template script \"" + m_parent->name() + "\"");
  }
  return symbol->type();
}

ska::Type ska::BridgeBuilderASTTemplateLooker::constructor() const {
    return m_name.empty() ? Type{} : variable(m_name);
}

ska::Type ska::BridgeBuilderASTTemplateLooker::field(const std::string& name) const {
    const Symbol* symbol;
    if (m_template == nullptr) {
        symbol = m_parent->symbols()[name];
    } else {
        symbol = (*m_template)[name];
    }
    
  if (symbol == nullptr) {
    throw std::runtime_error("unable to find field \"" + name + (m_name.empty() ? "" : "\" in constructor \"" + m_name) + "\" of template script \"" + m_parent->name() + "\"");
  }
  return symbol->type();
}

bool ska::BridgeBuilderASTTemplateLooker::hasField(const std::string& name) const {
  return (*m_template)[name] != nullptr;
}