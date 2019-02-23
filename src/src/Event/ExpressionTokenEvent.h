#pragma once

namespace ska {
	class ASTNode;
	class SymbolTable;

	class ExpressionTokenEvent {
	public:
		ExpressionTokenEvent(ASTNode& node, SymbolTable& s) : node(node), m_symbolTable(s) {}

		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

		SymbolTable& symbolTable() {
			return m_symbolTable;
		}

	private:
		ASTNode& node;
		SymbolTable& m_symbolTable;
	};
}
