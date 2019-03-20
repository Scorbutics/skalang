#pragma once
#include <cassert>

namespace ska {
    class ASTNode;
    class SymbolTable;
    
    class ImportTokenEvent {
	public:
		ImportTokenEvent(ASTNode& node, SymbolTable& s) : m_node(node), m_symbolTable(s) {
        }
		
        auto& rootNode() {
            return m_node;
		}

		const auto& rootNode() const {
            return m_node;
		}

		const SymbolTable& symbolTable() const {
			return m_symbolTable;
		}
	private:
		ASTNode& m_node;
        SymbolTable& m_symbolTable;
    };
}
