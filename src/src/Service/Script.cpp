#include "Script.h"	
#include "StatementParser.h"

const ska::Token& ska::Script::readPrevious(std::size_t offset) const {
	return m_input.readPrevious(offset);
}

bool ska::Script::canReadPrevious(std::size_t offset) const {
	return m_input.canReadPrevious(offset);
}

bool ska::Script::empty() const {
	return m_input.empty();
}

ska::Token ska::Script::actual() const {
	return m_input.actual();
}

const ska::Token& ska::Script::match(const Token& t) {
	return m_input.match(t);
}

const ska::Token& ska::Script::match(const TokenType& t) {
	return m_input.match(t);
}

bool ska::Script::expect(const Token& t) {
	return m_input.expect(t);
}

bool ska::Script::expect(const TokenType& type) const {
	return m_input.expect(type);
}


ska::ASTNodePtr ska::Script::parse(StatementParser& parser, bool listen) {
	if(!listen) {
		return parser.parse(*this);
	}

	m_symbols.listenParser(parser);
	auto result = parser.parse(*this);
	m_symbols.unlistenParser();
	return result;
}

ska::ASTNodePtr ska::Script::statement(StatementParser& parser) {
	return parser.statement(*this);
}

ska::ASTNodePtr ska::Script::optstatement(StatementParser& parser, const Token& mustNotBe) {
	return parser.optstatement(*this, mustNotBe);
}

ska::ASTNodePtr ska::Script::expr(StatementParser& parser) {
	return parser.expr(*this);
}

ska::ASTNodePtr ska::Script::optexpr(StatementParser& parser, const Token& mustNotBe) {
	return parser.optexpr(*this, mustNotBe);
}

ska::ASTNodePtr ska::Script::subParse(StatementParser& parser, std::ifstream& file) {
	return parser.subParse(file);
}