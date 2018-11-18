#include <iostream>

#include "LoggerConfigLang.h"
#include "SymbolTable.h"
#include "AST.h"

namespace ska {
    Type TypeBuilderBuildFromTokenType(const SymbolTable& symbols, const ASTNode& node) {
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

        SLOG_STATIC(ska::LogLevel::Error, ska::TypeBuilderOperator<Operator::LITERAL>) << "default type returned for node \"" << node.asString() << "\" of type " << TokenTypeSTR[static_cast<std::size_t>(node.tokenType())];

        return ExpressionType::VOID;
            
    }
       
 
}
