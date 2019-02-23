#pragma once
#include <cassert>

namespace ska {
    class ASTNode;
	class SymbolTable;

    enum class ReturnTokenEventType {
        START,
        BUILTIN,
        OBJECT
    };

    class ReturnTokenEvent {
	public:
        ReturnTokenEvent(SymbolTable& s) : m_type(ReturnTokenEventType::START), m_symbolTable(s) {}
		
        template<ReturnTokenEventType type>
        static ReturnTokenEvent Make(ASTNode& node, SymbolTable& s) {
            static_assert(type != ReturnTokenEventType::START);
            return ReturnTokenEvent { node, type, s };
        }
		
        auto& rootNode() {
			assert(m_node != nullptr);
            return *m_node;
		}

		const auto& rootNode() const {
			assert(m_node != nullptr);
            return *m_node;
		}

		const auto& type() const {
			return m_type;
		}

		SymbolTable& symbolTable() {
			return m_symbolTable;
		}

	private:
        ReturnTokenEvent(ASTNode& node, ReturnTokenEventType type, SymbolTable& s) : m_node(&node), m_type(type), m_symbolTable(s){
        }

		ASTNode* m_node = nullptr;
        ReturnTokenEventType m_type;
		SymbolTable& m_symbolTable;
    };
}
