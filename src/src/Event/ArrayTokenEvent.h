#pragma once

namespace ska {
	class ASTNode;

	enum class ArrayTokenEventType {
        DECLARATION,
		EXPRESSION,
        USE
	};

	class ArrayTokenEvent {
	public:
		ArrayTokenEvent(ASTNode& node, SymbolTable& s, ArrayTokenEventType type) : m_node(node), m_type(type), m_symbolTable(s) {}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		const auto& type() const {
			return m_type;
		}
		
		SymbolTable& symbolTable() {
			return m_symbolTable;
		}

	private:
		ArrayTokenEventType m_type;
		ASTNode& m_node;
		SymbolTable& m_symbolTable;
	};
}
