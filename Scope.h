#pragma once

#include <vector>
#include <unordered_map>

#include "AST.h"

namespace ska {

	class Scope {
	public:
		Scope() = default;

		Scope(Scope* parent) : 
			m_parent(parent) {
		}

		Scope& parent() {
			return m_parent == nullptr ? *this : *m_parent;
		}

		Scope& add() {
			m_subScopes.emplace_back();
			return m_subScopes.back();
		}

		void registerIdentifier(const std::string& identifier, std::unique_ptr<ASTNode> value) {
			m_identifierMap.emplace(identifier, std::move(value));
		}

	private:
		Scope* m_parent = nullptr;
		std::vector<Scope> m_subScopes;
		std::unordered_map<std::string, std::unique_ptr<ASTNode>> m_identifierMap;
	};
}