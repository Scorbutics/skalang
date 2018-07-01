#pragma once

#include <vector>
#include "Token.h"

namespace ska {

	class Scope {
	public:
		Scope() = default;

		Scope(Scope* parent) : 
			m_parent(parent) {
		}

		const std::vector<Token>& token() const {
			return m_nodeBlock;
		}

		Scope& parent() {
			return m_parent == nullptr ? *this : *m_parent;
		}

		Scope& add() {
			m_subScopes.emplace_back();
			return m_subScopes.back();
		}

		void addToken(Token token) {
			m_nodeBlock.push_back(std::move(token));
		}

	private:
		Scope* m_parent;
		std::vector<Token> m_nodeBlock;
		std::vector<Scope> m_subScopes;
	};
}