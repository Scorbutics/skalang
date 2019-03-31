#pragma once
#include <cassert>
#include <string>
#include "Interpreter/Value/ScriptPtr.h"

namespace ska {
    class ASTNode;
    class Script;
    
    class ImportTokenEvent {
	public:
		ImportTokenEvent(ASTNode& node, Script& s);
		
        auto& rootNode() {
            return m_node;
		}

		const auto& rootNode() const {
            return m_node;
		}

		const Script& script() const {
			return m_script;
		}

		const Script& bound() const {
			return *m_boundScript;
		}

		Script& bound() {
			return *m_boundScript;
		}

	private:
		ASTNode& m_node;
        Script& m_script;
		ScriptPtr m_boundScript;
    };
}
