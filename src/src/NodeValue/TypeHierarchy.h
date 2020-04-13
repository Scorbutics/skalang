#pragma once
#include "Type.h"
#include "Symbol.h"

namespace ska {
	
	struct TypeHierarchy {
		TypeHierarchy(Type t, const Symbol* link = nullptr) :
			type(std::move(t)),
			m_link(link) {}

		TypeHierarchy(const Symbol& symbol) :
			type(symbol.type()),
			m_link(&symbol) {}

		const Symbol* link() { return m_link; }

		Type type;
	private:
		const Symbol* m_link = nullptr;
	};
}
