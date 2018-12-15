#pragma once

#include "NodeValue/AST.h"

namespace ska {
    class ASTNode;
	
	enum class FunctionTokenEventType {
		DECLARATION_NAME,		
        DECLARATION_STATEMENT,
		CALL
	};

    class FunctionTokenEvent {
	public:

		FunctionTokenEvent(ASTNode& content, FunctionTokenEventType type, std::string name = "") :
			m_contentNode(content),
			m_name(std::move(name)),
			m_type(type) {
		}

		auto begin() { return m_contentNode.begin(); }
		auto end() { return m_contentNode.end(); }

		const auto begin() const { return m_contentNode.begin(); }
		const auto end() const { return m_contentNode.end(); }

		const auto& type() const {
			return m_type;
		}

		const auto& name() const {
			return m_name;
		}

		const auto& rootNode() const {
			return m_contentNode;
		}

		auto& rootNode() {
			return m_contentNode;
		}

	private:
		std::string m_name;
        ASTNode& m_contentNode;
	    FunctionTokenEventType m_type;
    };
}
