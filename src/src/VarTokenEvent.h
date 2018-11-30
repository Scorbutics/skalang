#pragma once

namespace ska {
    class ASTNode;

	enum class VarTokenEventType {
		PARAMETER_DECLARATION,
		VARIABLE_DECLARATION,
		AFFECTATION,
		USE
	};

    class VarTokenEvent {
	public:

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node, ASTNode& typeNode) {
			static_assert(type == VarTokenEventType::PARAMETER_DECLARATION);
			return VarTokenEvent { node, typeNode, type };
		}

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node) {
			static_assert(type != VarTokenEventType::PARAMETER_DECLARATION);
			return VarTokenEvent { node, type };
		}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		auto& typeNode() {
			assert(m_typeNode != nullptr);
			return *m_typeNode;
		}

		const auto& typeNode() const {
			assert(m_typeNode != nullptr);
			return *m_typeNode;
		}

		const auto& type() const {
			return m_type;
		}

		std::string name() const;

	private:
		VarTokenEvent(ASTNode& node, ASTNode& typeNode, VarTokenEventType type) : m_node(node), m_typeNode(&typeNode), m_type(type) {}
		VarTokenEvent(ASTNode& node, VarTokenEventType type = VarTokenEventType::VARIABLE_DECLARATION) : m_node(node), m_type(type) {}

		ASTNode& m_node;
		ASTNode* m_typeNode = nullptr;
		VarTokenEventType m_type;
    };
}
