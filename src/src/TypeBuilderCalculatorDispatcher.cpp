#include <iostream>

#include "LoggerConfigLang.h"
#include "SymbolTable.h"
#include "AST.h"

namespace ska {
	struct TypeBuilderBuildFromTokenTypeTag;

    Type TypeBuilderBuildFromTokenType(const SymbolTable& symbols, const ASTNode& node) {
        switch(node.tokenType()) {
            case TokenType::SYMBOL:
            case TokenType::SPACE:
            case TokenType::RANGE:
            case TokenType::DOT_SYMBOL:
				return Type{ ExpressionType::VOID };

            case TokenType::STRING:
				return Type{ ExpressionType::STRING };

            case TokenType::DIGIT: {
                const auto isDecimal = node.name().find_first_of('.') != std::string::npos;
				return Type{ isDecimal ? ExpressionType::FLOAT : ExpressionType::INT };
            }

            case TokenType::IDENTIFIER: {
                const auto symbol = symbols[node.name()];
				return symbol == nullptr ? Type{ ExpressionType::VOID } : symbol->getType();
            }
            case TokenType::RESERVED: {
				auto type = Type{};
                  if(node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::INT)])) {
					  type = Type{ ExpressionType::INT };
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::FLOAT)])) {
					  type = Type{ ExpressionType::FLOAT };
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::STRING)])) {
					  type = Type{ ExpressionType::STRING };
                  } else if(node.name() == "var") {
					  type = Type{ ExpressionType::OBJECT };
                  }
				  //handles arrays
				  if (node.size() == 1 && node[0].tokenType() == TokenType::ARRAY) {
					  type = Type{ ExpressionType::ARRAY }.add(type);
				  }
				  return type;
            }

            default:
                break;
        }

        SLOG_STATIC(ska::LogLevel::Error, TypeBuilderBuildFromTokenTypeTag) << "default type returned for node \"" << node.name() << "\" of type " << TokenTypeSTR[static_cast<std::size_t>(node.tokenType())];

		return Type{ ExpressionType::VOID };
            
    }
       
 
}
