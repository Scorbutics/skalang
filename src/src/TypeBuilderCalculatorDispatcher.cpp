#include <iostream>

#include "LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "TypeBuilderOperator.h"
#include "SymbolTable.h"
#include "AST.h"

#include "TypeBuilderFieldAccess.h"
#include "TypeBuilderFunctionCall.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderParameterDeclaration.h"
#include "TypeBuilderUnary.h"
#include "TypeBuilderBinary.h"
#include "TypeBuilderVariableAffectation.h"
#include "TypeBuilderVariableDeclaration.h"

namespace ska {
    Type TypeBuilderBuildFromTokenType(const SymbolTable& symbols, ASTNode& node) {
        switch(node.tokenType()) {
            case TokenType::SYMBOL:
            case TokenType::SPACE:
            case TokenType::RANGE:
            case TokenType::DOT_SYMBOL:
                return ExpressionType::VOID;

            case TokenType::STRING:
                return ExpressionType::STRING;

            case TokenType::DIGIT: {
                const auto isDecimal = node.asString().find_first_of('.') != std::string::npos;
                return isDecimal ? ExpressionType::FLOAT : ExpressionType::INT;
            }

            case TokenType::IDENTIFIER: {
                const auto symbol = symbols[node.asString()];
                return symbol == nullptr ? ExpressionType::VOID : symbol->getType();
            }
            case TokenType::RESERVED: {
                  if(node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::INT)])) {
                      return ExpressionType::INT;
                  } else if (node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::FLOAT)])) {
                    return ExpressionType::FLOAT;
                  } else if (node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::STRING)])) {
                        return ExpressionType::STRING;
                  } else if(node.asString() == "var") {
                      return ExpressionType::OBJECT;
                  }
            }

            default:
                break;
        }

        std::cout << "default type returned for node \"" << node.asString() << "\" of type " << TokenTypeSTR[static_cast<std::size_t>(node.tokenType())] << std::endl;

        return ExpressionType::VOID;
            
    }
    
    Type TypeBuilderDispatchCalculation(const SymbolTable& symbols, ASTNode& node) {
        if(node.type().has_value()) {
            return node.type().value();
        }

        if(node.op().has_value() && node.op().value() != Operator::LITERAL) {
            const auto& op = node.op().value();
            Type type;
            switch(op) {
                case Operator::FUNCTION_DECLARATION: type = TypeBuilderOperator<Operator::FUNCTION_DECLARATION>::build(symbols, node); break;
                case Operator::FUNCTION_CALL: type = TypeBuilderOperator<Operator::FUNCTION_CALL>::build(symbols, node); break;
                case Operator::FIELD_ACCESS: type = TypeBuilderOperator<Operator::FIELD_ACCESS>::build(symbols, node); break;
                case Operator::BINARY: type = TypeBuilderOperator<Operator::BINARY>::build(symbols, node);break;
                case Operator::PARAMETER_DECLARATION: type = TypeBuilderOperator<Operator::PARAMETER_DECLARATION>::build(symbols, node);break;
                case Operator::VARIABLE_DECLARATION: type = TypeBuilderOperator<Operator::VARIABLE_DECLARATION>::build(symbols, node);break;
                case Operator::UNARY: type = TypeBuilderOperator<Operator::UNARY>::build(symbols, node);break;
                case Operator::VARIABLE_AFFECTATION: type = TypeBuilderOperator<Operator::VARIABLE_AFFECTATION>::build(symbols, node);break;		
                default:
                    type = ExpressionType::VOID;
            }
            node.type() = type;
        } else {
            node.type() = TypeBuilderBuildFromTokenType(symbols, node);
        }
        return node.type().value();
    }
    
}
