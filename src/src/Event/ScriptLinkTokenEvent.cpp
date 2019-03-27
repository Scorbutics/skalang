#include "ScriptLinkTokenEvent.h"
#include "Service/Script.h"

ska::ScriptLinkTokenEvent::ScriptLinkTokenEvent(ASTNode& node, std::string scriptName, Script& scriptCaller) :
	m_node(node), 
	m_name(std::move(scriptName)),
	m_scriptCaller(scriptCaller),
	m_script(scriptCaller.subScript(m_name)) {
	assert(m_node.op() == Operator::SCRIPT_LINK); 
}

ska::Script& ska::ScriptLinkTokenEvent::script() {
	return m_scriptCaller;
}

const ska::Script& ska::ScriptLinkTokenEvent::script() const {
	return m_scriptCaller;
}

ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() {
	return m_node;
}

const ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() const {
	return m_node;
}

ska::SymbolTable& ska::ScriptLinkTokenEvent::symbolTable() {
	return m_scriptCaller.symbols();
}