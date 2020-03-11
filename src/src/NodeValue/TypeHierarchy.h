#pragma once
#include "Type.h"

namespace ska {
	struct SymbolLink {
		const Symbol* master = nullptr;
		const Symbol* implementation = nullptr;
	};
	
	struct TypeHierarchy {
		TypeHierarchy(Type t, SymbolLink link = {}) : 
			type(std::move(t)),
			link(std::move(link)) {}

		Type type;
		SymbolLink link;
	};
}
