#include "ScriptLinkTokenEvent.h"
#include "Service/Script.h"

ska::ScriptLinkTokenEvent::ScriptLinkTokenEvent(ASTNode& node, std::string scriptName, Script& parentScript) :
	m_node(node), 
	m_script(parentScript.subScript(scriptName)),
	m_name(std::move(scriptName)),
	m_symbolTable(parentScript.symbols()) {
	assert(m_node.op() == Operator::SCRIPT_LINK); 
}

ska::Script* ska::ScriptLinkTokenEvent::script() {
	return m_script.get();
}

const ska::Script* ska::ScriptLinkTokenEvent::script() const {
	return m_script.get();
}


ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() {
	return m_node;
}

const ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() const {
	return m_node;
}

ska::SymbolTable& ska::ScriptLinkTokenEvent::symbolTable() {
	return m_symbolTable;
}