#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <utility>
#include "AST.h"

namespace ska {

	class Scope {
	public:
		Scope() = default;

		Scope(Scope* parent) : 
			m_parent(parent) {
		}

		Scope(const Scope&) = delete;
		Scope(Scope&&) = default;

		Scope& parent() {
			return m_parent == nullptr ? *this : *m_parent;
		}

		Scope& add() {
			m_subScopes.emplace_back();
			return m_subScopes.back();
		}

		void registerIdentifier(const std::string& identifier, std::unique_ptr<ASTNode> value) {
			auto pair = std::make_pair(identifier, std::move(value));
			m_identifierMap.insert(std::move(pair));
		}

	private:
		Scope* m_parent = nullptr;
		std::vector<Scope> m_subScopes;
		std::unordered_map<std::string, std::unique_ptr<ASTNode>> m_identifierMap;
	};
}