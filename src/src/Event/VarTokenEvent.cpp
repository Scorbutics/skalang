#include "NodeValue/AST.h"
#include "VarTokenEvent.h"

std::string ska::VarTokenEvent::name() const
{
		switch (m_type) {
		case VarTokenEventType::AFFECTATION:
		//case VarTokenEventType::USE:
			return m_node[0].name();
		default:
			return m_node.name();
		}
	
}

const std::optional<ska::Type>& ska::VarTokenEvent::varType() const {
	return m_node.type();
}

std::string ska::VarTokenEvent::value() const {
	if (m_node.size() > 0) {
		return m_node[0].name();
	}
	return "";
}
