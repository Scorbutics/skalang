#include "AST.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/TypeBuilder/TypeBuildersContainer.h"

ska::ASTNode::ASTNode() :
	m_type(Type::MakeBuiltIn(ExpressionType::VOID)) {}

ska::ASTNode::ASTNode(Token t, ASTNodePtr l, ASTNodePtr r) :
	m_op(l != nullptr && r != nullptr ? Operator::BINARY : Operator::UNARY),
	m_token(std::move(t)) {
	if (l != nullptr) {
    	m_children.push_back(std::move(l));
	}

	if (r != nullptr) {
    	m_children.push_back(std::move(r));
	}

	if(m_token.isLiteral()) {
    	assert(m_op == Operator::UNARY);
    	m_op = Operator::LITERAL;
	}
}

ska::ASTNode:: ASTNode(Operator o, Token identifierToken, std::vector<ASTNodePtr> children) : 
	m_op(o),
	m_token(std::move(identifierToken)) {
	if(!children.empty()) {
    	m_children.reserve(children.size());
    	for(auto& child : children) {
        	if(child != nullptr) {
            	m_children.push_back(std::move(child));
        	}
    	}
	}
}

ska::ASTNode::ASTNode(Operator o, Token identifierToken) :
	m_op(std::move(o)),
	m_token(std::move(identifierToken)) {
}

const ska::Symbol* ska::ASTNode::typeSymbol() const { 
	//return m_symbol == nullptr ? nullptr : m_symbol->master(); 
	return !m_type.has_value() ? nullptr : TypeSymbolAccess(m_type.value());
}

const std::optional<ska::Type>& ska::ASTNode::type() const {
	//return m_symbol == nullptr ? m_type : m_symbol->type();
	return m_type;
}

void ska::ASTNode::linkSymbol(Symbol& symbol) { 
	m_symbol = &symbol;
	//refreshSymbolType();
	//m_type = symbol.type().type();
}
void ska::ASTNode::buildType(const TypeBuildersContainer& typeBuilders, ScriptAST& script) {
	if (m_type.has_value()) {
		return;
	}

	for(auto& child : m_children) {
    	child->buildType(typeBuilders, script);
	}
	auto& typeBuilder = typeBuilders[static_cast<std::size_t>(op())];
	assert(typeBuilder != nullptr && "Cannot calculate the node type (it might be an empty node)");
	m_type = typeBuilder->build(script, *this).type;
}
