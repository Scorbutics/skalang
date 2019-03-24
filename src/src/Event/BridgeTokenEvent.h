#pragma once
#include <cassert>
#include "NodeValue/Operator.h"
#include "Service/ScriptPtr.h"

namespace ska {
	class SymbolTable;
	class Script;
	class ASTNode;

	class BridgeTokenEvent {
	public:
		BridgeTokenEvent(ASTNode& node, std::string scriptName, Script& parentScript);

		Script* script();
		const Script* script() const;

		ASTNode& rootNode();
		const ASTNode& rootNode() const;

		const std::string& name() const { return m_name; }

		SymbolTable& symbolTable();

	private:
		ASTNode& m_node;
		ScriptPtr m_script;
		std::string m_name;
		SymbolTable& m_symbolTable;
	};
}
