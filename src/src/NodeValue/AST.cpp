#include "AST.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/TypeBuilder/TypeBuilderLiteral.h"

ska::ASTNode::ASTNode(Token t, ASTNodePtr l, ASTNodePtr r) :
    m_op(l != nullptr && r != nullptr ? Operator::BINARY : Operator::UNARY),
    token(std::move(t)) {
    if (l != nullptr) {
        m_children.push_back(std::move(l));
    }

    if (r != nullptr) {
        m_children.push_back(std::move(r));
    }

    if(token.isLiteral()) {
        assert(m_op == Operator::UNARY);
        m_op = Operator::LITERAL;
		m_typeBuilder = std::make_unique<TypeBuilderOperator<Operator::LITERAL>>();
	} else {
		if (m_op == Operator::BINARY) {
			m_typeBuilder = std::make_unique<TypeBuilderOperator<Operator::BINARY>>();
		} else {
			m_typeBuilder = std::make_unique<TypeBuilderOperator<Operator::UNARY>>();
		}
	}
}

ska::ASTNode:: ASTNode(Operator o, Token identifierToken, std::vector<ASTNodePtr> children) : 
    m_op(o),
    token(std::move(identifierToken)) {
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
    token(std::move(identifierToken)) {
}

void ska::ASTNode::buildType(Parser& parser, const SymbolTable& symbols) {
	if (m_type.has_value()) {
		return;
	}

	for(auto& child : m_children) {
        child->buildType(parser, symbols);
    }
	assert(m_typeBuilder != nullptr && "Cannot calculate the node type (it might be an empty node)");
    m_type = m_typeBuilder->build(parser, symbols, *this);
}

void ska::ASTNode::buildValue(NodeValue value) {
	m_value = std::move(value);
}
