#pragma once

#include "NodeValue/Type.h"

namespace ska {
    class ASTNode;
	class SymbolTable;

	enum class VarTokenEventType {
		FUNCTION_DECLARATION,
		PARAMETER_DECLARATION,
		VARIABLE_DECLARATION,
		AFFECTATION,
		USE
	};

    class VarTokenEvent {
	public:
		static VarTokenEvent MakeFunction(ASTNode& node, SymbolTable& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::FUNCTION_DECLARATION };
		}

		static VarTokenEvent MakeParameter(ASTNode& node, ASTNode& typeNode, SymbolTable& s) {
			return VarTokenEvent { node, s, typeNode, VarTokenEventType::PARAMETER_DECLARATION };
		}

		static VarTokenEvent MakeUse(ASTNode& node, SymbolTable& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::USE };
		}

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node, SymbolTable& s) {
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

		SymbolTable& symbolTable() {
			return m_symbolTable;
		}

		const std::optional<Type>& varType() const;
		std::string value() const;
		std::string name() const;

	private:
		VarTokenEvent(ASTNode& node, SymbolTable& s, ASTNode& typeNode, VarTokenEventType type) : m_node(node), m_typeNode(&typeNode), m_type(type), m_symbolTable(s) {}
		VarTokenEvent(ASTNode& node, SymbolTable& s, VarTokenEventType type = VarTokenEventType::VARIABLE_DECLARATION) : m_node(node), m_type(type), m_symbolTable(s) {}

		ASTNode& m_node;
		ASTNode* m_typeNode = nullptr;
		VarTokenEventType m_type;
		SymbolTable& m_symbolTable;
    };
}
