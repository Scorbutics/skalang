#pragma once

namespace ska {
	class ASTNode;
	class BridgeTokenEvent {
	public:
		BridgeTokenEvent(ASTNode& node) : m_node(node) {}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

	private:
		ASTNode& m_node;
	};
}
