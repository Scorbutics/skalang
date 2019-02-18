#pragma once

#include "NodeValue/ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "NodeValue/Token.h"

namespace ska {
	class StatementParser;
	
	class Script {
	public:
		Script(const std::vector<ska::Token>& input, std::size_t startIndex = 0) :
			m_input(input, startIndex) {
		}
		virtual ~Script() = default;
	
        const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;
		bool empty() const;
        Token actual() const;
        const Token& match(const Token& t);
		const Token& match(const TokenType& t);
        bool expect(const Token& t);
		bool expect(const TokenType& type) const;

		ASTNodePtr parse(StatementParser& parser, bool listen = true);

		ASTNodePtr statement(StatementParser& parser);
        ASTNodePtr optstatement(StatementParser& parser, const Token& mustNotBe = Token{});

		ASTNodePtr expr(StatementParser& parser);
		ASTNodePtr optexpr(StatementParser& parser, const Token& mustNotBe = Token{});

		//ASTNodePtr subParse(StatementParser& parser, std::ifstream& file);

		SymbolTable& symbols() {
			return m_symbols;
		}

	private:
		TokenReader m_input;
		SymbolTable m_symbols;
		ASTNodePtr m_ast;
	};
}
