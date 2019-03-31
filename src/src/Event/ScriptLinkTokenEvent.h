#pragma once
#include <cassert>
#include "NodeValue/Operator.h"
#include "Interpreter/Value/ScriptPtr.h"

namespace ska {
	class SymbolTable;
	class Script;
	class ASTNode;

	class ScriptLinkTokenEvent {
	public:
		ScriptLinkTokenEvent(ASTNode& node, std::string scriptName, Script& scriptCaller);

		Script& script();
		const Script& script() const;

		Script* bound() { return m_script.get(); }
		const Script* bound() const { return m_script.get(); }

		ASTNode& rootNode();
		const ASTNode& rootNode() const;

		const std::string& name() const { return m_name; }

		SymbolTable& symbolTable();

	private:
		ASTNode& m_node;
		std::string m_name;
		Script& m_scriptCaller;
		ScriptPtr m_script;		
	};
}
