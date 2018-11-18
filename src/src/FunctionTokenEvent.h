#pragma once

#include "AST.h"

namespace ska {
    class ASTNode;
	
	enum class FunctionTokenEventType {
		DECLARATION_PARAMETERS,
        DECLARATION_STATEMENT,
		CALL
	};

    class FunctionTokenEvent {
	public:

		FunctionTokenEvent(std::string name, ASTNode& node, ASTNode* returnTypeNode, FunctionTokenEventType type) :
			m_name(std::move(name)),
			m_node(node),
			m_returnType(returnTypeNode),
			m_type(type) {
		}

		auto begin() { return m_node.begin(); }
		auto end() { return m_node.end(); }

		const auto begin() const { return m_node.begin(); }
		const auto end() const { return m_node.end(); }

		const auto& type() const {
			return m_type;
		}

		const auto& name() const {
			return m_name;
		}

		const auto& rootNode() const {
			return m_node;
		}

		auto& rootNode() {
			return m_node;
		}

		auto& returnTypeNode() {
			return m_returnType;
		}

		const auto& returnTypeNode() const {
			return m_returnType;
		}

	private:
        std::string m_name;
        ASTNode& m_node;
		ASTNode* m_returnType = nullptr;
	    FunctionTokenEventType m_type;
    };
}
