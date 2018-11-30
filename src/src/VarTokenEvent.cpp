#include "AST.h"
#include "VarTokenEvent.h"

std::string ska::VarTokenEvent::name() const
{
		switch (m_type) {
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE:
			return m_node[0].name();
		default:
			return m_node.name();
		}
	
}
