#pragma once
#include <cassert>

namespace ska {
    class ASTNode;
    class Script;
    
    class ImportTokenEvent {
	public:
		ImportTokenEvent(ASTNode& node, Script& s) : m_node(node), m_script(s) {
        }
		
        auto& rootNode() {
            return m_node;
		}

		const auto& rootNode() const {
            return m_node;
		}

		const Script& script() const {
			return m_script;
		}
	private:
		ASTNode& m_node;
        Script& m_script;
    };
}
