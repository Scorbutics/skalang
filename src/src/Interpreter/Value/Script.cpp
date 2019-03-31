#include <memory>
#include "Script.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"

const ska::Token& ska::Script::readPrevious(std::size_t offset) const {
	return m_handle->m_input.readPrevious(offset);
}

ska::ScriptPtr ska::Script::subScript(const std::string& name) {
	return existsInCache(name) ? std::make_unique<Script>(m_cache, name, std::vector<Token>{}) : nullptr;
}

ska::ASTNode& ska::Script::fromBridge(std::vector<BridgeMemory> bindings) {
	assert(m_handle->m_ast == nullptr && "Script built from a bridge must be empty");

	pushNestedMemory();
	auto functionListNodes = bindings.empty() ? std::vector<ASTNodePtr>() : std::vector<ASTNodePtr>(bindings.size());
	for (auto& bridgeFunction : bindings) {
		auto functionName = bridgeFunction->node->name();
		auto functionVarDeclarationNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(Token{ functionName, TokenType::IDENTIFIER }), std::move(bridgeFunction->node));
		functionListNodes.push_back(std::move(functionVarDeclarationNode));
		m_handle->m_currentMemory->emplace(functionName, NodeValue{ std::move(bridgeFunction) });
	}
	endNestedMemory();

	m_handle->m_ast = ASTFactory::MakeNode<Operator::BLOCK>(std::move(functionListNodes));
	m_handle->m_bridged = true;
	return *m_handle->m_ast;
}

bool ska::Script::canReadPrevious(std::size_t offset) const {
	return m_handle->m_input.canReadPrevious(offset);
}

bool ska::Script::empty() const {
	return m_handle->m_input.empty();
}

ska::Token ska::Script::actual() const {
	return m_handle->m_input.actual();
}

const ska::Token& ska::Script::match(const Token& t) {
	return m_handle->m_input.match(t);
}

const ska::Token& ska::Script::match(const TokenType& t) {
	return m_handle->m_input.match(t);
}

bool ska::Script::expect(const Token& t) {
	return m_handle->m_input.expect(t);
}

bool ska::Script::expect(const TokenType& type) const {
	return m_handle->m_input.expect(type);
}

ska::ScriptPtr ska::Script::subParse(StatementParser& parser, const std::string& name, std::ifstream& file) {
	return parser.subParse(m_cache, name, file);
}

void ska::Script::parse(StatementParser& parser, bool listen) {
	if(m_inCache) {
		//No more parsing if script is already in-cache.
		return;
	}

	if(!listen) {
		m_handle->m_ast = parser.parse(*this);
		return;
	}

	m_handle->m_symbols.listenParser(parser);
	auto result = parser.parse(*this);
	m_handle->m_symbols.unlistenParser();
	m_handle->m_ast = std::move(result);
	return;
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

