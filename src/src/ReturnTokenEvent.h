#pragma once

namespace ska {
    class ASTNode;

    enum class ReturnTokenEventType {
        BUILTIN,
        OBJECT
    };

    class ReturnTokenEvent {
	public:
		ReturnTokenEvent(ASTNode& node, ReturnTokenEventType type) : node(node), m_type(type) {}
		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

		const auto& type() const {
			return m_type;
		}

	private:
		ASTNode& node;
        ReturnTokenEventType m_type;
    };
}
