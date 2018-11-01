#pragma once

namespace ska {
    class ASTNode;

	enum class VarTokenEventType {
		DECLARATION,
		AFFECTATION,
		USE
	};

    class VarTokenEvent {
	public:
		VarTokenEvent(ASTNode& node, VarTokenEventType type = VarTokenEventType::DECLARATION) : node(node), m_type(type) {}

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
		VarTokenEventType m_type = VarTokenEventType::DECLARATION;
    };
}
