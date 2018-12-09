#pragma once

namespace ska {
    class ASTNode;

	enum class VarTokenEventType {
		FUNCTION_DECLARATION,
		PARAMETER_DECLARATION,
		VARIABLE_DECLARATION,
		AFFECTATION,
		USE
	};

    class VarTokenEvent {
	public:
		static VarTokenEvent MakeFunction(ASTNode& node) {
			return VarTokenEvent{ node, VarTokenEventType::FUNCTION_DECLARATION };
		}

		static VarTokenEvent MakeParameter(ASTNode& node, ASTNode& typeNode) {
			return VarTokenEvent { node, typeNode, VarTokenEventType::PARAMETER_DECLARATION };
		}

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node) {
			static_assert(type != VarTokenEventType::PARAMETER_DECLARATION && type != VarTokenEventType::FUNCTION_DECLARATION);
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

		const std::optional<Type>& varType() const;
		std::string value() const;
		std::string name() const;

	private:
		VarTokenEvent(ASTNode& node, ASTNode& typeNode, VarTokenEventType type) : m_node(node), m_typeNode(&typeNode), m_type(type) {}
		VarTokenEvent(ASTNode& node, VarTokenEventType type = VarTokenEventType::VARIABLE_DECLARATION) : m_node(node), m_type(type) {}

		ASTNode& m_node;
		ASTNode* m_typeNode = nullptr;
		VarTokenEventType m_type;
    };
}
