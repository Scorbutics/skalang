#include "Script.h"	
#include "StatementParser.h"

void ska::Script::parse(StatementParser& parser) {
	m_symbols.listenParser(parser);
	parser.parse(*this);
	m_symbols.unlistenParser();
}

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
