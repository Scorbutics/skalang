#pragma once

namespace ska {
	class ASTNode;

	enum class ArrayTokenEventType {
		DECLARATION,
		USE
	};

	class ArrayTokenEvent {
	public:
		ArrayTokenEvent(ASTNode& node, ArrayTokenEventType type) : m_node(node), m_type(type) {}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		const auto& type() const {
			return m_type;
		}

	private:
		ArrayTokenEventType m_type;
		ASTNode& m_node;
	};
}
