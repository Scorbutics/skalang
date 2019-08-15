#include "AST.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"

ska::ASTNode::ASTNode(): 
	m_type(Type::MakeBuiltIn(ExpressionType::VOID)) {
}

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

void ska::ASTNode::buildType(const TypeBuildersContainer& typeBuilders, const ScriptAST& script) {
	if (m_type.has_value()) {
		return;
	}

	for(auto& child : m_children) {
        child->buildType(typeBuilders, script);
    }
	auto& typeBuilder = typeBuilders[static_cast<std::size_t>(op())];
	assert(typeBuilder != nullptr && "Cannot calculate the node type (it might be an empty node)");
    m_type = typeBuilder->build(script, *this);
}
