#include "TypeBuilder.h"
#include "AST.h"

ska::Type ska::TypeBuilder::TypeBuilder(const SymbolTable& symbolTable) : 
    m_symbols(symbolTable) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) {
    calculateExpressionType(event.node[0]);
    return true;
}

void ska::TypeBuilder::calculateExpressionType(ASTNode& node) const {
	if(node.type.has_value()) {
		return node.type.value();
	}

    if(node.op.has_value() && node.op.value() != Operator::LITERAL) {
		const auto& op = node.op.value();
		switch(op) {
            case Operator::FUNCTION_DECLARATION: return TypeBuilder<Operator::FUNCTION_DECLARATION>::build(node);
			case Operator::FUNCTION_CALL: return buildTypeFunctionCall(node);
			case Operator::FIELD_ACCESS: return buildTypeFieldAccess(node);
			case Operator::BINARY: return buildTypeBinary(node);
			case Operator::PARAMETER_DECLARATION: return buildTypeParameterDeclaration(node);
			case Operator::VARIABLE_DECLARATION: return buildTypeVariableDeclaration(node);
			case Operator::UNARY: return buildTypeUnary(node);
            case Operator::VARIABLE_AFFECTATION: return buildTypeVariableAffectation(node);		
            default:
				return ExpressionType::VOID;
		}
	}

	return buildTypeFromTokenType(node);
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	calculateExpressionType(event.node);
    return true;
}

bool ska::SemanticTypeChecker::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
		    calculateExpressionType(event.node);
        } break;

    }
    return true;
}

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(ASTNode& node) {
    auto functionType = Type{ ExpressionType::FUNCTION };
    for (auto index = 0u; index < node.size(); index++) {
        auto varType = getExpressionType(node[index]);
        if(varType == ExpressionType::OBJECT) {
            varType.name(node.asString());
        }
        functionType.add(std::move(varType));
    }
#ifdef SKALANG_LOG_TYPE_BUILDER
    std::cout << "function declaration \""<< node.asString() <<"\" with type "<< functionType.asString() << std::endl;
#endif
    return functionType;
}
