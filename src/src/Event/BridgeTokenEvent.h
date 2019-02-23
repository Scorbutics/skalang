#pragma once
#include <cassert>
#include "NodeValue/Operator.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class BridgeTokenEvent {
	public:
		BridgeTokenEvent(ASTNode& node, SymbolTable& s) : m_node(node), m_symbolTable(s) { assert(m_node.op() == Operator::BRIDGE); }

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		SymbolTable& symbolTable() {
			return m_symbolTable;
		}

	private:
		ASTNode& m_node;
		SymbolTable& m_symbolTable;
	};
}
