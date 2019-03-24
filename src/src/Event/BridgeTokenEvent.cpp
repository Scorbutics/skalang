#include "BridgeTokenEvent.h"
#include "Service/Script.h"

ska::BridgeTokenEvent::BridgeTokenEvent(ASTNode& node, std::string scriptName, Script& parentScript) :
	m_node(node), 
	m_script(parentScript.subScript(scriptName)),
	m_name(std::move(scriptName)),
	m_symbolTable(parentScript.symbols()) {
	assert(m_node.op() == Operator::BRIDGE); 
}

ska::Script* ska::BridgeTokenEvent::script() {
	return m_script.get();
}

const ska::Script* ska::BridgeTokenEvent::script() const {
	return m_script.get();
}


ska::ASTNode& ska::BridgeTokenEvent::rootNode() {
	return m_node;
}

const ska::ASTNode& ska::BridgeTokenEvent::rootNode() const {
	return m_node;
}

ska::SymbolTable& ska::BridgeTokenEvent::symbolTable() {
	return m_symbolTable;
}