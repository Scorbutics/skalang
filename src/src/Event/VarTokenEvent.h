#pragma once

#include "NodeValue/Type.h"

namespace ska {
    class ASTNode;
	class Script;

	enum class VarTokenEventType {
		FUNCTION_DECLARATION,
		PARAMETER_DECLARATION,
		VARIABLE_DECLARATION,
		AFFECTATION,
		USE
	};

    class VarTokenEvent {
	public:
		static VarTokenEvent MakeFunction(ASTNode& node, Script& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::FUNCTION_DECLARATION };
		}

		static VarTokenEvent MakeParameter(ASTNode& node, ASTNode& typeNode, Script& s) {
			return VarTokenEvent { node, s, typeNode, VarTokenEventType::PARAMETER_DECLARATION };
		}

		static VarTokenEvent MakeUse(ASTNode& node, Script& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::USE };
		}

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node, Script& s) {
			static_assert(type != VarTokenEventType::PARAMETER_DECLARATION && type != VarTokenEventType::FUNCTION_DECLARATION);
			return VarTokenEvent { node, s, type };
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

		auto& script() {
			return m_script;
		}

		const auto& script() const {
			return m_script;
		}

		const std::optional<Type>& varType() const;
		std::string value() const;
		std::string name() const;

	private:
		VarTokenEvent(ASTNode& node, Script& s, ASTNode& typeNode, VarTokenEventType type) : m_node(node), m_typeNode(&typeNode), m_type(type), m_script(s) {}
		VarTokenEvent(ASTNode& node, Script& s, VarTokenEventType type = VarTokenEventType::VARIABLE_DECLARATION) : m_node(node), m_type(type), m_script(s) {}

		ASTNode& m_node;
		ASTNode* m_typeNode = nullptr;
		VarTokenEventType m_type;
		Script& m_script;
    };
}
