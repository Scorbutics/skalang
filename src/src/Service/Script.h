#pragma once

#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"

namespace ska {
	class Script {
	public:
		Script() = default;
		virtual ~Script() = default;
		
		void parse();
		bool empty();

	private:
		TokenReader m_input;
		SymbolTable m_symbols;
		ASTNodePtr m_ast;
	};
}
