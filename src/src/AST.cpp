#include "AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

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

void ska::ASTNode::buildType(const SymbolTable& symbols) {
    //TODO virtual member functions ?
    //m_type = m_typeBuildUnit.build();
    for(auto& child : m_children) {
        child->buildType(symbols);
    }
    m_type = TypeBuilderDispatchCalculation(symbols, *this);
}
