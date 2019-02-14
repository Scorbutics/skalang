#pragma once

#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"

namespace ska {
	class Script {
	public:
		Script() = default;
		virtual ~Script() = default;
		
		void parse();
		bool empty() const;
        Token actual() const;
        void match(const Token& t);
        bool expect(const Token& t);

	private:
		TokenReader m_input;
		SymbolTable m_symbols;
		ASTNodePtr m_ast;
	};
}
