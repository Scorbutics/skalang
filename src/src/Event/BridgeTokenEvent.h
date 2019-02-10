#pragma once
#include <cassert>
#include "NodeValue/Operator.h"

namespace ska {
	class ASTNode;
	class BridgeTokenEvent {
	public:
		BridgeTokenEvent(ASTNode& node) : m_node(node) { assert(m_node.op() == Operator::BRIDGE); }

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
